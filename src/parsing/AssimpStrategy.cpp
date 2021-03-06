//TODO: mesh will be a field inside the aiNode
//TODO: update scene node to use composition with mesh instead of inheritance

#include "AssimpStrategy.h"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "glm/glm.hpp"

#include "../model/coNode.h"
#include "../log/Log.h"

#define uint unsigned int

const aiScene *parsingScene;

coNode *parseNode(aiNode *aiNode);

void parseLights(const aiScene *aiScene, coNode* targetScene);

glm::mat4 convertMatrix(const aiMatrix4x4 &aiMat);

coMesh *parseMesh(uint meshIndex);

inline glm::vec3 convertColor(aiColor3D c) {
    return glm::vec3{c.r, c.g, c.b};
}

inline glm::vec3 convertVec3(aiVector3D v) {
    return glm::vec3{v.x, v.y, v.z};
}

void AssimpStrategy::execute() {
    CO_LOG_INFO("Starting to parse");
    Assimp::Importer importer;

    const aiScene *scene = importer.ReadFile(m_filepath,
                                             aiProcess_CalcTangentSpace |
                                             aiProcess_Triangulate |
                                             aiProcess_JoinIdenticalVertices |
                                             aiProcess_GenSmoothNormals |
                                             aiProcess_FindInvalidData);
    parsingScene = scene;

    CO_LOG_INFO("Finished parsing");

    CO_LOG_INFO("Starting nodes conversion");
    m_parsed = parseNode(scene->mRootNode);
    CO_LOG_INFO("Finished converting nodes");

    if (scene->HasLights()) {
        CO_LOG_INFO("Starting lights conversion");
        parseLights(scene, m_parsed);
    }
}

void parseLights(const aiScene *aiScene, coNode *targetScene) {
    for (int i = 0; i < aiScene->mNumLights; i++) {

        coLight *ourLight = new coLight{};
        aiLight *theirLight = aiScene->mLights[i];

        switch (theirLight->mType) {
            case aiLightSourceType::aiLightSource_DIRECTIONAL:
                ourLight->m_direction = convertVec3(theirLight->mDirection);
                ourLight->m_lightType = lightType::DIRECTIONAL;
                break;

            case aiLightSourceType::aiLightSource_POINT:
                ourLight->m_position = convertVec3(theirLight->mPosition);
                ourLight->m_lightType = lightType::OMNI;
                break;

            case aiLightSourceType::aiLightSource_SPOT:
                ourLight->m_innerConeAngle = theirLight->mAngleInnerCone;
                ourLight->m_outerConeAngle = theirLight->mAngleOuterCone;
                ourLight->m_direction = convertVec3(theirLight->mDirection);
                ourLight->m_position = convertVec3(theirLight->mPosition);
                ourLight->m_lightType = lightType::SPOT;
                break;
            default:
                ourLight->m_lightType = lightType::UNDEF;
                CO_LOG_WARN("Light {} is of unknown type", theirLight->mName.C_Str());
        }

        ourLight->m_linearAttenuation = theirLight->mAttenuationLinear;
        ourLight->m_constantAttenuation = theirLight->mAttenuationConstant;
        ourLight->m_quadraticAttenuation = theirLight->mAttenuationQuadratic;

        ourLight->m_ambient = convertColor(theirLight->mColorAmbient);
        ourLight->m_diffuse = convertColor(theirLight->mColorDiffuse);
        ourLight->m_specular = convertColor(theirLight->mColorSpecular);

        coNode* owner = targetScene->findInChildren(theirLight->mName.C_Str());
        if(owner == nullptr){
            CO_LOG_WARN("Light {} has no owner node", theirLight->mName.C_Str());
        }else{
            owner->getLights().push_back(ourLight);
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
        CO_LOG_INFO("found {} mesh to add as children", aiNode->mNumMeshes);

        for (int i = 0; i < aiNode->mNumMeshes; i++) {
            node->getMMeshes().push_back(parseMesh(aiNode->mMeshes[i]));
        }
    }

    for (int i = 0; i < aiNode->mNumChildren; i++) {
        CO_LOG_INFO("parsing child #{} of {}", i, node->m_name);

        node->addChild(parseNode(aiNode->mChildren[i]));
    }

    return node;
}

glm::mat4 convertMatrix(const aiMatrix4x4 &aiMat) {
    return {
            aiMat.a1, aiMat.b1, aiMat.c1, aiMat.d1,
            aiMat.a2, aiMat.b2, aiMat.c2, aiMat.d2,
            aiMat.a3, aiMat.b3, aiMat.c3, aiMat.d3,
            aiMat.a4, aiMat.b4, aiMat.c4, aiMat.d4
    };
}

coMesh *parseMesh(uint meshIndex) {
    // TODO look for tangent, bitangents, colors, bones, materials
    CO_LOG_INFO("converting mesh");

    aiMesh *mesh = parsingScene->mMeshes[meshIndex];

    std::vector<uint> triangles;
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec3> mapping;

    triangles.reserve(mesh->mNumFaces * 3);
    vertices.reserve(mesh->mNumVertices);
    normals.reserve(mesh->mNumVertices);
    mapping.reserve(mesh->mNumVertices);

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

        mapping[i] = convertVec3(mesh->mTextureCoords[0][i]);
        CO_LOG_TRACE("parsed uv: {}, {}", mapping[i].x, mapping[i].y);
    }

    auto newMesh = new coMesh();

    newMesh->setMVertices(vertices);
    newMesh->setMIndices(triangles);
    newMesh->setMNormals(normals);
    newMesh->setMTextureCoordinates(mapping);

    CO_LOG_INFO("mesh converted");

    return newMesh;
}