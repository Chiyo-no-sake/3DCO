
#include <model/coLight.h>
#include <utils/XmlPropertiesParser.h>
#include <glm/ext/scalar_constants.hpp>
#include "AssimpStrategy.h"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "../log/Log.h"
#include "utils/file_utils.h"
#include "utils/material_utils.h"
#include "utils/mesh_utils.h"
#include "utils/types_utils.h"

const aiScene *parsingScene;

coScene *parsed;

coNode *parseNode(aiNode *aiNode);

void parseMaterials(coScene *targetScene);

coMeshData *parseMeshData(unsigned int meshIndex);

void AssimpStrategy::execute() {
    Assimp::Importer importer;
    m_parsed = new coScene();
    parsed = m_parsed;

    CO_LOG_INFO("Initiated scene parsing with Assimp...");

    const aiScene *scene = importer.ReadFile(m_filepath,
                                             aiProcess_CalcTangentSpace |
                                             aiProcess_Triangulate |
                                             aiProcess_JoinIdenticalVertices |
                                             aiProcess_FindInvalidData);

    parsed->m_directory = getDirectoryFor(m_filepath);
    CO_LOG_TRACE("Scene directory: {}", parsed->m_directory);

    if (scene == nullptr) {
        CO_LOG_ERR("Cannot not parse {}", m_filepath);
        return;
    }

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

int getLightIndexFor(aiNode *aiNode) {
    for (auto i = 0; i < parsingScene->mNumLights; i++) {
        if (parsingScene->mLights[i]->mName == aiNode->mName)
            return i;
    }

    return -1;
}

std::string getMaterialNameFor(unsigned int meshIndex) {
    unsigned int matIndex = parsingScene->mMeshes[meshIndex]->mMaterialIndex;
    std::string matName = parsingScene->mMaterials[matIndex]->GetName().C_Str();
    return matName;
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

            CO_LOG_INFO("found {} LODs", aiNode->mNumMeshes);
            for (unsigned int i = 0; i < aiNode->mNumMeshes; i++) {
                mesh->getLODs().push_back(parseMeshData(aiNode->mMeshes[i]));
            }

            mesh->m_matName = getMaterialNameFor(aiNode->mMeshes[0]);
            CO_LOG_INFO("Found material for mesh {}, name: {}", mesh->m_name, mesh->m_matName);

            mesh->m_bboxMax = mesh->findBoundingBoxMax();
            mesh->m_bboxMin = mesh->findBoundingBoxMin();
            mesh->m_radius = mesh->computeMeshRadius();

            CO_LOG_TRACE("Maximum bounding box: {}, {}, {}", mesh->m_bboxMax.x, mesh->m_bboxMax.y, mesh->m_bboxMax.z);
            CO_LOG_TRACE("Minimum bounding box: {}, {}, {}", mesh->m_bboxMin.x, mesh->m_bboxMin.y, mesh->m_bboxMin.z);
            CO_LOG_TRACE("Mesh radius: {}", mesh->m_radius);

            XmlPropertiesParser *parser = XmlPropertiesParser::getInstance();

            CO_LOG_INFO("Parsing physics properties");

            float volume = meshVolume(mesh->getLODs()[0]);
            mesh->m_mass = parser->getProperty(mesh->m_matName, "density") * volume;

            std::string meshNamePostfix = aiNode->mName.C_Str();
            meshNamePostfix = meshNamePostfix.substr(meshNamePostfix.size()-4);

            if(meshNamePostfix == ".stc"){
                mesh->m_physicsType = PHYS_STATIC;
                mesh->m_hullType = HULL_UNDEFINED;
            }
            else if(meshNamePostfix == ".dyn"){
                mesh->m_physicsType = PHYS_DYNAMIC;
                mesh->m_hullType = HULL_CUSTOM;
            }
            else
                mesh->m_physicsType = PHYS_UNDEFINED;

            CO_LOG_TRACE("Mass: {} ", mesh->m_mass);

            mesh->m_bounciness = parser->getProperty(mesh->m_matName, "bounciness");
            CO_LOG_TRACE("Bounciness: {} ", mesh->m_bounciness);

            mesh->m_staticFriction = parser->getProperty(mesh->m_matName, "friction-static");
            CO_LOG_TRACE("Static friction: {} ", mesh->m_staticFriction);

            mesh->m_dynamicFriction = parser->getProperty(mesh->m_matName, "friction-dynamic");
            CO_LOG_TRACE("Dynamic friction: {} ", mesh->m_dynamicFriction);

            mesh->m_linearDamping = parser->getProperty(mesh->m_matName, "damping-linear");
            CO_LOG_TRACE("Linear damping: {} ", mesh->m_linearDamping);

            mesh->m_angularDamping = parser->getProperty(mesh->m_matName, "damping-angular");
            CO_LOG_TRACE("Angular damping: {} ", mesh->m_angularDamping);

            mesh->m_centerOfMass = verticesMean(mesh->getLODs()[0]);
            CO_LOG_TRACE("Center of mass: {}, {}, {}",
                         mesh->m_centerOfMass.x,
                         mesh->m_centerOfMass.y,
                         mesh->m_centerOfMass.z);

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

coMeshData *parseMeshData(unsigned int meshIndex) {
    CO_LOG_INFO("converting mesh");

    aiMesh *mesh = parsingScene->mMeshes[meshIndex];

    unsigned int *triangles;
    glm::vec3 *vertices;
    glm::vec3 *normals;
    glm::vec3 *mapping;
    glm::vec3 *tangents;
    glm::vec3 *bitangents;
    bool hasTexCoords = true;
    bool hasTangents = true;
    bool hasBitangents = true;

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

    if(mesh->mTextureCoords[0] == nullptr){
        CO_LOG_WARN("Mesh has no texture coordinates. Did you forget UV unwrapping?");
        mapping = nullptr;
        hasTexCoords = false;
    }

    if(mesh->mTangents == nullptr){
        CO_LOG_WARN("Mesh has no tangents data. Did you forget UV unwrapping?");
        tangents = nullptr;
        hasTangents = false;

    }

    if(mesh->mBitangents == nullptr){
        CO_LOG_WARN("Mesh has no bitangents data. Did you forget UV unwrapping?");
        bitangents = nullptr;
        hasBitangents = false;
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

        if(hasTexCoords) {
            mapping[i] = convertVec3(mesh->mTextureCoords[0][i]);
            CO_LOG_TRACE("parsed uv: {}, {}", mapping[i].x, mapping[i].y);
        }

        // parse tangents
        if(hasTangents) {
            tangents[i] = convertVec3(mesh->mTangents[i]);
            CO_LOG_TRACE("parsed tan: {}, {}", tangents[i].x, tangents[i].y);
        }

        // parse bitangents
        if(hasBitangents) {
            bitangents[i] = convertVec3(mesh->mBitangents[i]);
            CO_LOG_TRACE("parsed bi-tan: {}, {}", bitangents[i].x, bitangents[i].y);
        }
    }

    auto newMesh = new coMeshData();

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


void parseMaterials(coScene *targetScene) {
    for (unsigned int i = 0; i < parsingScene->mNumMaterials; i++) {
        auto material = new coMaterial();
        auto srcMaterial = parsingScene->mMaterials[i];

        material->m_name = srcMaterial->GetName().C_Str();
        targetScene->getMMaterials().emplace(material->m_name, material);

        CO_LOG_INFO("Parsing material {}", material->m_name);

        material->m_albedo = getAlbedoColor(srcMaterial);
        material->m_emission = getEmissionColor(srcMaterial);
        material->m_roughness = getRoughnessValue(srcMaterial);
        material->m_metalness = getMetalnessValue(srcMaterial);
        material->m_transparency = 1 - getOpacityValue(srcMaterial);

        // ########################### Getting material colors #########################################################

        CO_LOG_TRACE("Converted values:\n"
                     "\talbedo: {}, {}, {}\n"
                     "\temission: {}, {}, {}\n"
                     "\troughness: {}\n"
                     "\ttransparency: {}\n"
                     "\tmetalness: {}",
                     material->m_albedo.r, material->m_albedo.g, material->m_albedo.b,
                     material->m_emission.r, material->m_emission.g, material->m_emission.b,
                     material->m_roughness,
                     material->m_transparency,
                     material->m_metalness);
        // ########################### Converting material colors ######################################################

        // ########################### Gathering textures ##############################################################

        std::string *diffuseTexturePath = tryGetDiffusePath(srcMaterial);
        std::string *opacityTexturePath = tryGetOpacityPath(srcMaterial);
        std::string *normalTexturePath = tryGetNormalPath(srcMaterial);
        std::string *metallicTexturePath = tryGetMetallicPath(srcMaterial);
        std::string *roughnessTexturePath = tryGetRoughnessPath(srcMaterial);

        if (diffuseTexturePath == nullptr) {
            CO_LOG_TRACE("No diffuse texture for material {}", material->m_name);
            material->setAlbedoMap(std::string("[none]"));
        } else {
            if (opacityTexturePath != nullptr) {
                CO_LOG_INFO("Opacity map found, including in albedo");
                material->setAlbedoMap(convertTexture(parsingScene, *diffuseTexturePath, material->m_name, ALBEDO, *opacityTexturePath));
            } else {
                material->setAlbedoMap(convertTexture(parsingScene, *diffuseTexturePath, material->m_name, ALBEDO));
            }
        }

        if (normalTexturePath == nullptr) {
            CO_LOG_TRACE("No normal map for material {}", material->m_name);
            material->setNormalMap(std::string("[none]"));
        } else {
            material->setNormalMap(convertTexture(parsingScene, *normalTexturePath, material->m_name, NORMAL));
        }


        material->setHeightMap(std::string("[none]"));


        if(metallicTexturePath == nullptr) CO_LOG_TRACE("No metallic map for material {}", material->m_name);
        else{
            material->setMetalnessMap(convertTexture(parsingScene, *metallicTexturePath, material->m_name, METAL));
        }
        material->setMetalnessMap(std::string{"[none]"});


        if(roughnessTexturePath == nullptr) CO_LOG_TRACE("No roughness map for material {}", material->m_name);
        else{
            material->setRoughnessMap(convertTexture(parsingScene, *roughnessTexturePath, material->m_name, ROUGH));
        }
        material->setRoughnessMap(std::string{"[none]"});

        delete normalTexturePath;
        delete metallicTexturePath;
        delete roughnessTexturePath;
        delete opacityTexturePath;
        delete diffuseTexturePath;
    }
}