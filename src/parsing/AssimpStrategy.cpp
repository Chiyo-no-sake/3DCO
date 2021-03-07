//TODO: mesh will be a field inside the aiNode
//TODO: update scene node to use composition with mesh instead of inheritance

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
void parseMaterials(const aiScene *srcScene, coScene* targetScene);

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

    CO_LOG_INFO("Starting nodes conversion");
    m_parsed->m_rootNode = parseNode(scene->mRootNode);
    CO_LOG_INFO("Finished converting nodes");

    if (scene->HasLights()) {
        CO_LOG_INFO("Starting lights conversion");
        parseLights(scene, m_parsed);
        CO_LOG_INFO("Finished lights conversion");

    }

    if(scene->HasMaterials()) {
        CO_LOG_INFO("Starting materials conversion");
        parseMaterials(scene, m_parsed);
        CO_LOG_INFO("Finished materials conversion");
    }
}

void parseLights(const aiScene *srcScene, coScene *targetScene) {
    for (int i = 0; i < srcScene->mNumLights; i++) {
        auto ourLight = new coLight{};
        aiLight *theirLight = srcScene->mLights[i];

        ourLight->m_name = theirLight->mName.C_Str();
        CO_LOG_INFO("Converting light {}", ourLight->m_name);


        switch (theirLight->mType) {
            case aiLightSourceType::aiLightSource_DIRECTIONAL:
                ourLight->m_direction = convertVec3(theirLight->mDirection);
                ourLight->m_type = lightType::DIRECTIONAL;
                CO_LOG_TRACE("light type {}", coLight::typeToString(ourLight->m_type));
                CO_LOG_TRACE("light direction {}, {}, {}", ourLight->m_direction.x, ourLight->m_direction.y,
                             ourLight->m_direction.z);
                break;

            case aiLightSourceType::aiLightSource_POINT:
                ourLight->m_position = convertVec3(theirLight->mPosition);
                ourLight->m_type = lightType::OMNI;
                CO_LOG_TRACE("light type {}", coLight::typeToString(ourLight->m_type));
                CO_LOG_TRACE("light position {}, {}, {}", ourLight->m_position.x, ourLight->m_position.y,
                             ourLight->m_position.z);
                break;

            case aiLightSourceType::aiLightSource_SPOT:
                ourLight->m_innerConeAngle = theirLight->mAngleInnerCone;
                ourLight->m_outerConeAngle = theirLight->mAngleOuterCone;
                ourLight->m_direction = convertVec3(theirLight->mDirection);
                ourLight->m_position = convertVec3(theirLight->mPosition);
                ourLight->m_type = lightType::SPOT;

                CO_LOG_TRACE("light type {}", coLight::typeToString(ourLight->m_type));
                CO_LOG_TRACE("light position {}, {}, {}", ourLight->m_position.x, ourLight->m_position.y,
                             ourLight->m_position.z);
                CO_LOG_TRACE("light direction {}, {}, {}", ourLight->m_direction.x, ourLight->m_direction.y,
                             ourLight->m_direction.z);
                CO_LOG_TRACE("light innerCone {}", ourLight->m_innerConeAngle);
                CO_LOG_TRACE("light outerCone {}", ourLight->m_innerConeAngle);

                break;
            default:
                ourLight->m_type = lightType::UNDEF;
                CO_LOG_WARN("Light {} is of unknown type", theirLight->mName.C_Str());
        }

        ourLight->m_linearAttenuation = theirLight->mAttenuationLinear;
        ourLight->m_constantAttenuation = theirLight->mAttenuationConstant;
        ourLight->m_quadraticAttenuation = theirLight->mAttenuationQuadratic;

        ourLight->m_ambient = convertColor(theirLight->mColorAmbient);
        ourLight->m_diffuse = convertColor(theirLight->mColorDiffuse);
        ourLight->m_specular = convertColor(theirLight->mColorSpecular);

        coNode *owner = targetScene->m_rootNode->findInChildren(theirLight->mName.C_Str());
        if (owner == nullptr) {
            CO_LOG_WARN("Light {} has no owner node", theirLight->mName.C_Str());
        } else {
            CO_LOG_TRACE("Added light {} to owner {}", ourLight->m_name, owner->m_name);
            owner->getMLights().push_back(ourLight);
        }
    }
}

void parseMaterials(const aiScene* srcScene, coScene* targetScene) {
    for(int i=0;i<srcScene->mNumMaterials; i++){
        auto material = new coMaterial();


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

        for (int i = 0; i < aiNode->mNumMeshes; i++) {
            node->getMMeshes().push_back(parseMesh(aiNode->mMeshes[i]));
        }
    }

    node->m_numChildren = aiNode->mNumChildren;
    CO_LOG_INFO("found {}", node->m_numChildren);

    for (int i = 0; i < aiNode->mNumChildren; i++) {
        CO_LOG_INFO("parsing child #{} of {}", i, node->m_name);

        node->addChild(parseNode(aiNode->mChildren[i]));
    }

    return node;
}

coMesh *parseMesh(uint meshIndex) {
    // TODO look for bones, materials
    CO_LOG_INFO("converting mesh");

    aiMesh *mesh = parsingScene->mMeshes[meshIndex];

    std::vector<uint> triangles;
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec3> mapping;
    std::vector<glm::vec3> tangents;
    std::vector<glm::vec3> bitangents;

    triangles.reserve(mesh->mNumFaces * 3);
    vertices.reserve(mesh->mNumVertices);
    normals.reserve(mesh->mNumVertices);
    mapping.reserve(mesh->mNumVertices);
    tangents.reserve(mesh->mNumVertices);
    bitangents.reserve(mesh->mNumVertices);

    CO_LOG_INFO("num triangles: {}", mesh->mNumFaces);
    CO_LOG_INFO("num vertices: {}", mesh->mNumVertices);

    if (mesh->mNumFaces == 0) {
        CO_LOG_WARN("Mesh has no face");
    }

    if (mesh->mNumVertices == 0) {
        CO_LOG_WARN("Mesh has no vertices");
    }

    for (int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];

        for (uint j = 0; j < face.mNumIndices; j++) {
            triangles[i * 3 + j] = face.mIndices[j];
            CO_LOG_TRACE("parsed index: {}", triangles[i * 3 + j]);
        }
    }

    for (int i = 0; i < mesh->mNumVertices; i++) {
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

    newMesh->m_numVertices = mesh->mNumVertices;
    newMesh->setMVertices(vertices);
    newMesh->setMIndices(triangles);
    newMesh->setMNormals(normals);
    newMesh->setMTextureCoordinates(mapping);

    CO_LOG_INFO("mesh converted");

    return newMesh;
}

