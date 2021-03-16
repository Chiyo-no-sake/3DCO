
#include <model/coLight.h>
#include "AssimpStrategy.h"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "glm/glm.hpp"
#include "../log/Log.h"

const aiScene *parsingScene;

coScene *parsed;

coNode *parseNode(aiNode *aiNode);

void parseMaterials(coScene *targetScene);

glm::mat4 convertMatrix(const aiMatrix4x4 &aiMat);

coMeshData *parseMesh(unsigned int meshIndex);

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
    parsed = m_parsed;

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
        parseMaterials(m_parsed);
        CO_LOG_INFO("Finished materials conversion");
    }

    CO_LOG_INFO("Starting nodes conversion");
    m_parsed->m_rootNode = parseNode(scene->mRootNode);
    CO_LOG_INFO("Finished converting nodes");
}

void parseLightData(coLight *targetNode, int lightIndex) {
    aiLight *theirLight = parsingScene->mLights[lightIndex];

    targetNode->m_name = theirLight->mName.C_Str();
    CO_LOG_INFO("Converting light {}", targetNode->m_name);


    switch (theirLight->mType) {
        case aiLightSourceType::aiLightSource_DIRECTIONAL:
            targetNode->m_direction = convertVec3(theirLight->mDirection);
            targetNode->m_type = lightType::DIRECTIONAL;
            CO_LOG_TRACE("light type {}", coLight::typeToString(targetNode->m_type));
            CO_LOG_TRACE("light direction {}, {}, {}", targetNode->m_direction.x, targetNode->m_direction.y,
                         targetNode->m_direction.z);
            break;

        case aiLightSourceType::aiLightSource_POINT:
            targetNode->m_position = convertVec3(theirLight->mPosition);
            targetNode->m_type = lightType::OMNI;
            CO_LOG_TRACE("light type {}", coLight::typeToString(targetNode->m_type));
            CO_LOG_TRACE("light position {}, {}, {}", targetNode->m_position.x, targetNode->m_position.y,
                         targetNode->m_position.z);
            break;

        case aiLightSourceType::aiLightSource_SPOT:
            targetNode->m_cutoff = theirLight->mAngleInnerCone / 2;
            targetNode->m_direction = convertVec3(theirLight->mDirection);
            targetNode->m_position = convertVec3(theirLight->mPosition);
            targetNode->m_type = lightType::SPOT;
            targetNode->m_spotExponent = 128 * (1 - theirLight->mAngleInnerCone / theirLight->mAngleOuterCone);

            CO_LOG_TRACE("light type {}", coLight::typeToString(targetNode->m_type));
            CO_LOG_TRACE("light position {}, {}, {}", targetNode->m_position.x, targetNode->m_position.y,
                         targetNode->m_position.z);
            CO_LOG_TRACE("light direction {}, {}, {}", targetNode->m_direction.x, targetNode->m_direction.y,
                         targetNode->m_direction.z);

            CO_LOG_TRACE("light cutoff {}", targetNode->m_cutoff);
            if (targetNode->m_cutoff > 90.0 || targetNode->m_cutoff < 0.0)
                CO_LOG_WARN("light cutoff value outside OpenGL-defined bounds");

            CO_LOG_TRACE("light exponent {}", targetNode->m_spotExponent);

            break;
        default:
            targetNode->m_type = lightType::UNDEF;
            CO_LOG_WARN("Light {} is of unknown type", theirLight->mName.C_Str());
    }

    targetNode->m_linearAttenuation = theirLight->mAttenuationLinear;
    targetNode->m_constantAttenuation = theirLight->mAttenuationConstant;
    targetNode->m_quadraticAttenuation = theirLight->mAttenuationQuadratic;

    CO_LOG_TRACE("light constant, linear, quadratic attenuation {}, {}, {}",
                 targetNode->m_constantAttenuation,
                 targetNode->m_linearAttenuation,
                 targetNode->m_quadraticAttenuation);

    targetNode->m_ambient = convertColor(theirLight->mColorAmbient);
    targetNode->m_diffuse = convertColor(theirLight->mColorDiffuse);
    targetNode->m_specular = convertColor(theirLight->mColorSpecular);

    CO_LOG_TRACE("light ambient component {} {} {}", targetNode->m_ambient.r, targetNode->m_ambient.g,
                 targetNode->m_ambient.b);
    CO_LOG_TRACE("light diffuse component {} {} {}", targetNode->m_diffuse.r, targetNode->m_diffuse.g,
                 targetNode->m_diffuse.b);
    CO_LOG_TRACE("light specular component {} {} {}", targetNode->m_specular.r, targetNode->m_specular.g,
                 targetNode->m_specular.b);

    targetNode->m_shadowCasting = 1;
    targetNode->m_volumetricLighting = 0;

    CO_LOG_TRACE("light shadow casting set to 1");
    CO_LOG_TRACE("light volumetric lighting set to 0");
}

void parseMaterials(coScene *targetScene) {
    for (unsigned int i = 0; i < parsingScene->mNumMaterials; i++) {
        auto material = new coMaterial();
        auto srcMaterial = parsingScene->mMaterials[i];

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

int getLightIndexFor(aiNode *aiNode) {
    for (auto i = 0; i < parsingScene->mNumLights; i++) {
        if (parsingScene->mLights[i]->mName == aiNode->mName)
            return i;
    }

    return -1;
}

coNode *parseNode(aiNode *aiNode) {
    CO_LOG_INFO("converting {}", aiNode->mName.C_Str());

    coNode *node = nullptr;

    // check if node is light and parse
    int lightIndex = getLightIndexFor(aiNode);
    if (lightIndex != -1) {
        CO_LOG_INFO("node {} is a light", aiNode->mName.C_Str());
        auto light = new coLight();
        parseLightData(light, lightIndex);
        node = (coNode *) light;
    }

    // check if node is mesh and parse
    if (node == nullptr) {
        if (aiNode->mNumMeshes != 0) {
            CO_LOG_INFO("node {} is a mesh", aiNode->mName.C_Str());
            auto mesh = new coMesh();
            mesh->m_numLods = aiNode->mNumMeshes;
            // node has meshes, add them to mesh list

            CO_LOG_INFO("found {} LODs", aiNode->mNumMeshes);

            for (unsigned int i = 0; i < aiNode->mNumMeshes; i++) {
                mesh->getLODs().push_back(parseMesh(aiNode->mMeshes[i]));
            }

            node = (coNode *) mesh;

            parsed->getMMeshes().push_back(mesh);

        }
    }

    // if node has not a type it's a dummy node
    if (node == nullptr) {
        CO_LOG_INFO("node {} is a dummy", aiNode->mName.C_Str());
        node = new coNode{};
    }

    node->m_name = aiNode->mName.C_Str();
    node->m_transform = convertMatrix(aiNode->mTransformation);
    node->m_numChildren = aiNode->mNumChildren;

    CO_LOG_INFO("Node {} has {} children", node->m_name, node->m_numChildren);

    for (unsigned int i = 0; i < aiNode->mNumChildren; i++) {
        CO_LOG_INFO("parsing child #{} of {}", i, node->m_name);

        node->addChild(parseNode(aiNode->mChildren[i]));
    }

    return node;
}

coMeshData *parseMesh(unsigned int meshIndex) {
    // TODO
    //  1) look for bones
    //  2) look for material, find in scene and set id

    CO_LOG_INFO("converting mesh");

    aiMesh *mesh = parsingScene->mMeshes[meshIndex];

    unsigned int *triangles;
    glm::vec3 *vertices;
    glm::vec3 *normals;
    glm::vec3 *mapping;
    glm::vec3 *tangents;
    glm::vec3 *bitangents;

    triangles = static_cast<unsigned int *>(malloc(mesh->mNumFaces * 3 * sizeof(unsigned int)));
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

        for (unsigned int j = 0; j < face.mNumIndices; j++) {
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

    auto newMesh = new coMeshData();

    newMesh->m_materialName = parsingScene->mMaterials[mesh->mMaterialIndex]->GetName().C_Str();
    newMesh->m_numVertices = mesh->mNumVertices;
    newMesh->m_numIndices = totalIndices;
    newMesh->setMBitangents(bitangents);
    newMesh->setMTangents(tangents);
    newMesh->setMVertices(vertices);
    newMesh->setMIndices(triangles);
    newMesh->setMNormals(normals);
    newMesh->setMTextureCoordinates(mapping);


    CO_LOG_INFO("mesh converted");

    return newMesh;
}

