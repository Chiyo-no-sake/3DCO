#include "AssimpStrategy.h"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "glm/glm.hpp"
#include "../log/Log.h"

#define uint unsigned int

const aiScene *parsingScene;

coNode *parseNode(aiNode *aiNode);

void parseLights(const aiScene *srcScene, coScene *targetScene);

void parseMaterials(const aiScene *srcScene, coScene *targetScene);

glm::mat4 convertMatrix(const aiMatrix4x4 &aiMat);

coMesh *parseMesh(uint meshIndex);

inline glm::vec3 convertColor(aiColor3D c) {
    return glm::vec3{c.r, c.g, c.b};
}

inline glm::vec3 convertVec3(aiVector3D v) {
    return glm::vec3{v.x, v.y, v.z};
}

inline glm::mat4 convertMatrix(const aiMatrix4x4 &aiMat) {
    return {
            aiMat.a1, aiMat.b1, aiMat.c1, aiMat.d1,
            aiMat.a2, aiMat.b2, aiMat.c2, aiMat.d2,
            aiMat.a3, aiMat.b3, aiMat.c3, aiMat.d3,
            aiMat.a4, aiMat.b4, aiMat.c4, aiMat.d4
    };
}

void AssimpStrategy::execute() {
    Assimp::Importer importer;
    m_parsed = new coScene();

    CO_LOG_INFO("Starting to parse");

    const aiScene *scene = importer.ReadFile(m_filepath,
                                             aiProcess_CalcTangentSpace |
                                             aiProcess_Triangulate |
                                             aiProcess_JoinIdenticalVertices |
                                             aiProcess_GenSmoothNormals |
                                             aiProcess_FindInvalidData);
    parsingScene = scene;
    CO_LOG_INFO("Finished parsing");

    if (scene->HasMaterials()) {
        CO_LOG_INFO("Starting materials conversion");
        parseMaterials(scene, m_parsed);
        CO_LOG_INFO("Finished materials conversion");
    }

    CO_LOG_INFO("Starting nodes conversion");
    m_parsed->m_rootNode = parseNode(scene->mRootNode);
    CO_LOG_INFO("Finished converting nodes");

    if (scene->HasLights()) {
        CO_LOG_INFO("Starting lights conversion");
        parseLights(scene, m_parsed);
        CO_LOG_INFO("Finished lights conversion");
    }
}

void parseLights(const aiScene *srcScene, coScene *targetScene) {
    for (unsigned int i = 0; i < srcScene->mNumLights; i++) {
        auto ourLight = new coLight{};
        aiLight *theirLight = srcScene->mLights[i];

        ourLight->m_name = theirLight->mName.C_Str();
        CO_LOG_INFO("Converting light {}", ourLight->m_name);


        switch (theirLight->mType) {
            case aiLightSourceType::aiLightSource_DIRECTIONAL:
                ourLight->m_direction = convertVec3(theirLight->mDirection);
                ourLight->m_type = lightType::DIRECTIONAL;
                CO_LOG_TRACE("light type {}", coLight::typeToString(ourLight->m_type));
                CO_LOG_TRACE("light direction {}, {}, {}", ourLight->m_direction.x, ourLight->m_direction.y, ourLight->m_direction.z);
                break;

            case aiLightSourceType::aiLightSource_POINT:
                ourLight->m_position = convertVec3(theirLight->mPosition);
                ourLight->m_type = lightType::OMNI;
                CO_LOG_TRACE("light type {}", coLight::typeToString(ourLight->m_type));
                CO_LOG_TRACE("light position {}, {}, {}", ourLight->m_position.x, ourLight->m_position.y, ourLight->m_position.z);
                break;

            case aiLightSourceType::aiLightSource_SPOT:
                ourLight->m_cutoff = theirLight->mAngleInnerCone / 2;
                ourLight->m_direction = convertVec3(theirLight->mDirection);
                ourLight->m_position = convertVec3(theirLight->mPosition);
                ourLight->m_type = lightType::SPOT;
                ourLight->m_spotExponent = 128 * (1 - theirLight->mAngleInnerCone / theirLight->mAngleOuterCone);

                CO_LOG_TRACE("light type {}", coLight::typeToString(ourLight->m_type));
                CO_LOG_TRACE("light position {}, {}, {}", ourLight->m_position.x, ourLight->m_position.y, ourLight->m_position.z);
                CO_LOG_TRACE("light direction {}, {}, {}", ourLight->m_direction.x, ourLight->m_direction.y, ourLight->m_direction.z);

                CO_LOG_TRACE("light cutoff {}", ourLight->m_cutoff);
                if (ourLight->m_cutoff > 90.0 || ourLight->m_cutoff < 0.0)
                    CO_LOG_WARN("light cutoff value outside OpenGL-defined bounds");

                CO_LOG_TRACE("light exponent {}", ourLight->m_spotExponent);

                break;
            default:
                ourLight->m_type = lightType::UNDEF;
                CO_LOG_WARN("Light {} is of unknown type", theirLight->mName.C_Str());
        }

        ourLight->m_linearAttenuation = theirLight->mAttenuationLinear;
        ourLight->m_constantAttenuation = theirLight->mAttenuationConstant;
        ourLight->m_quadraticAttenuation = theirLight->mAttenuationQuadratic;

        CO_LOG_TRACE("light constant, linear, quadratic attenuation {}, {}, {}",
                ourLight->m_constantAttenuation,
                ourLight->m_linearAttenuation,
                ourLight->m_quadraticAttenuation);

        ourLight->m_ambient = convertColor(theirLight->mColorAmbient);
        ourLight->m_diffuse = convertColor(theirLight->mColorDiffuse);
        ourLight->m_specular = convertColor(theirLight->mColorSpecular);

        CO_LOG_TRACE("light ambient component {} {} {}", ourLight->m_ambient.r, ourLight->m_ambient.g, ourLight->m_ambient.b);
        CO_LOG_TRACE("light diffuse component {} {} {}", ourLight->m_diffuse.r, ourLight->m_diffuse.g , ourLight->m_diffuse.b);
        CO_LOG_TRACE("light specular component {} {} {}", ourLight->m_specular.r, ourLight->m_specular.g, ourLight->m_specular.b);

        ourLight->m_shadowCasting = 1;
        ourLight->m_volumetricLighting = 0;

        CO_LOG_TRACE("light shadow casting set to 1");
        CO_LOG_TRACE("light volumetric lighting set to 0");

        coNode *owner = targetScene->m_rootNode->findInChildren(theirLight->mName.C_Str());
        if (owner == nullptr) {
            CO_LOG_WARN("Light {} has no owner node", theirLight->mName.C_Str());
        } else {
            CO_LOG_TRACE("Added light {} to owner {}", ourLight->m_name, owner->m_name);
            owner->getMLights().push_back(ourLight);
        }
    }
}

void parseMaterials(const aiScene *srcScene, coScene *targetScene) {
    for (unsigned int i = 0; i < srcScene->mNumMaterials; i++) {
        auto material = new coMaterial();
        auto srcMaterial = srcScene->mMaterials[i];

        material->m_name = srcMaterial->GetName().C_Str();

        // TODO:
        //  1) search for maps
        //  2) handle pbr data that is inaccessible from assimp
        //  3) add material to scene

        aiColor3D srcAmbient;

        // get material albedo
        if (AI_SUCCESS == srcMaterial->Get(AI_MATKEY_COLOR_AMBIENT, srcAmbient)) {
            material->m_albedo = convertColor(srcAmbient) / 0.2f;
        } else {
            CO_LOG_WARN("Material {} has no ambient color", material->m_name);
        }

    }
}

coNode *parseNode(aiNode *aiNode) {
    auto node = new coNode{};

    CO_LOG_INFO("converting {}", aiNode->mName.C_Str());

    node->m_name = aiNode->mName.C_Str();
    node->m_transform = convertMatrix(aiNode->mTransformation);

    node->m_numLods = aiNode->mNumMeshes;

    // node has meshes, add them to mesh list
    if (aiNode->mNumMeshes != 0) {
        CO_LOG_INFO("found {} LODs", aiNode->mNumMeshes);

        for (unsigned int i = 0; i < aiNode->mNumMeshes; i++) {
            node->getMMeshes().push_back(parseMesh(aiNode->mMeshes[i]));
        }
    }


    node->m_numChildren = aiNode->mNumChildren;
    CO_LOG_INFO("found {}", node->m_numChildren);

    for (unsigned int i = 0; i < aiNode->mNumChildren; i++) {
        CO_LOG_INFO("parsing child #{} of {}", i, node->m_name);

        node->addChild(parseNode(aiNode->mChildren[i]));
    }

    return node;
}

coMesh *parseMesh(uint meshIndex) {
    // TODO
    //  1) look for bones
    //  2) look for material, find in scene and set id

    CO_LOG_INFO("converting mesh");

    aiMesh *mesh = parsingScene->mMeshes[meshIndex];

    uint *triangles;
    glm::vec3 *vertices;
    glm::vec3 *normals;
    glm::vec3 *mapping;
    glm::vec3 *tangents;
    glm::vec3 *bitangents;

    triangles = static_cast<unsigned int *>(malloc(mesh->mNumFaces * 3 * sizeof(uint)));
    vertices = static_cast<glm::vec3 *>(malloc(mesh->mNumVertices * sizeof(glm::vec3)));
    normals = static_cast<glm::vec3 *>(malloc(mesh->mNumVertices * sizeof(glm::vec3)));
    mapping = static_cast<glm::vec3 *>(malloc(mesh->mNumVertices * sizeof(glm::vec3)));
    tangents = static_cast<glm::vec3 *>(malloc(mesh->mNumVertices * sizeof(glm::vec3)));
    bitangents = static_cast<glm::vec3 *>(malloc(mesh->mNumVertices * sizeof(glm::vec3)));

    CO_LOG_INFO("num triangles: {}", mesh->mNumFaces);
    CO_LOG_INFO("num vertices: {}", mesh->mNumVertices);

    if (mesh->mNumFaces == 0) {
        CO_LOG_WARN("Mesh has no face");
    }

    if (mesh->mNumVertices == 0) {
        CO_LOG_WARN("Mesh has no vertices");
    }

    unsigned int totalIndices = 0;

    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];

        for (uint j = 0; j < face.mNumIndices; j++) {
            triangles[i * 3 + j] = face.mIndices[j];
            totalIndices++;
            CO_LOG_TRACE("parsed index: {}", triangles[i * 3 + j]);
        }
    }

    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        // parse vertex
        vertices[i] = convertVec3(mesh->mVertices[i]);
        CO_LOG_TRACE("parsed vertex: {}, {}, {}", vertices[i].x, vertices[i].y, vertices[i].z);


        // parse normals
        normals[i] = convertVec3(mesh->mNormals[i]);
        CO_LOG_TRACE("parsed normal: {}, {}, {}", vertices[i].x, vertices[i].y, vertices[i].z);

        // parse uvs
        mapping[i] = convertVec3(mesh->mTextureCoords[0][i]);
        CO_LOG_TRACE("parsed uv: {}, {}", mapping[i].x, mapping[i].y);

        // parse tangents
        tangents[i] = convertVec3(mesh->mTangents[i]);
        CO_LOG_TRACE("parsed tan: {}, {}", tangents[i].x, tangents[i].y);

        // parse bitangents
        bitangents[i] = convertVec3(mesh->mBitangents[i]);
        CO_LOG_TRACE("parsed bi-tan: {}, {}", bitangents[i].x, bitangents[i].y);

    }

    auto newMesh = new coMesh();

    newMesh->m_materialName = parsingScene->mMaterials[mesh->mMaterialIndex]->GetName().C_Str();
    newMesh->m_numVertices = mesh->mNumVertices;
    newMesh->m_numIndices = totalIndices;
    newMesh->setMVertices(vertices);
    newMesh->setMIndices(triangles);
    newMesh->setMNormals(normals);
    newMesh->setMTextureCoordinates(mapping);

    CO_LOG_INFO("mesh converted");

    return newMesh;
}

