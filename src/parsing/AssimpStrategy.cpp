
#include <model/coLight.h>
#include "AssimpStrategy.h"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "../log/Log.h"
#include <FreeImage.h>

#define uint unsigned int

const aiScene *parsingScene;

coNode *parseNode(aiNode *aiNode);

void parseMaterials(coScene *targetScene);

glm::mat4 convertMatrix(const aiMatrix4x4 &aiMat);

coMeshData *parseMesh(uint meshIndex);

inline glm::vec3 convertColor(const aiColor3D& c) {
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

    if(scene == nullptr) {
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
            //TODO cutoff should beb mAngleInnerCone
            targetNode->m_cutoff = theirLight->mAngleOuterCone;
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
    for (int i = 0; i < parsingScene->mNumLights; i++) {
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
        }
    }

    // if node has not a type it's a dummy node
    if (node == nullptr) {
        CO_LOG_INFO("node {} is dummy", aiNode->mName.C_Str());
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

coMeshData *parseMesh(uint meshIndex) {
    // TODO
    //  look for material, find in scene and set id

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

bool writeToTGA(const aiTexture* tex, std::string outPath){
    FreeImage_Initialise();

    FIBITMAP* bitmap = FreeImage_Allocate(tex->mWidth, tex->mHeight, 32);

    for(int y=0; y<tex->mHeight; y++){
        for(int x=0; x<tex->mWidth; x++){
            aiTexel texel = tex->pcData[x+y];
            RGBQUAD color;
            color.rgbReserved = texel.a;
            color.rgbRed = texel.r;
            color.rgbGreen = texel.g;
            color.rgbBlue = texel.b;

            bool result = FreeImage_SetPixelColor(bitmap, x, y, &color);
            if(!result) return false;
        }
    }

    bool result = FreeImage_Save(FIF_TARGA, bitmap, outPath.c_str(), TARGA_DEFAULT);

    FreeImage_DeInitialise();
    return result;
}

void parseMaterials(coScene *targetScene) {
    for (unsigned int i = 0; i < parsingScene->mNumMaterials; i++) {
        auto material = new coMaterial();
        auto srcMaterial = parsingScene->mMaterials[i];

        material->m_name = srcMaterial->GetName().C_Str();
        targetScene->getMMaterials().emplace(material->m_name, material);

        CO_LOG_INFO("Parsing material {}", material->m_name);

        aiColor3D srcAmbient = aiColor3D(.1, .1, .1);
        aiColor3D srcSpecular = aiColor3D(1, 1, 1);
        aiColor3D srcDiffuse = aiColor3D(.3, .3, .3);
        aiColor3D srcEmission = aiColor3D(0, 0, 0);

        float shininess;
        float opacity;

        //TODO: glTF get directly pbr data

        // ########################### Getting material colors #########################################################
        if (AI_SUCCESS != srcMaterial->Get(AI_MATKEY_COLOR_AMBIENT, srcAmbient)) {
            CO_LOG_WARN("Material {} has no ambient color. Setting default", material->m_name);
        }
        CO_LOG_TRACE("Material ambient set to: {}, {}, {}", srcAmbient.r, srcAmbient.g, srcAmbient.b);

        if (AI_SUCCESS != srcMaterial->Get(AI_MATKEY_COLOR_SPECULAR, srcSpecular)) {
            CO_LOG_WARN("Material {} has no specular color. Setting default", material->m_name);
        }
        CO_LOG_TRACE("Material specular set to: {}, {}, {}", srcSpecular.r, srcSpecular.g, srcSpecular.b);

        if (AI_SUCCESS != srcMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, srcDiffuse)) {
            CO_LOG_WARN("Material {} has no diffuse color. Setting default", material->m_name);
        }
        CO_LOG_TRACE("Material diffuse set to: {}, {}, {}", srcDiffuse.r, srcDiffuse.g, srcDiffuse.b);

        if (AI_SUCCESS != srcMaterial->Get(AI_MATKEY_COLOR_EMISSIVE, srcEmission)) {
            CO_LOG_WARN("Material {} has no emissive color. Setting default", material->m_name);
        }
        CO_LOG_TRACE("Material emission set to: {}, {}, {}", srcEmission.r, srcEmission.g, srcEmission.b);

        if (AI_SUCCESS != srcMaterial->Get(AI_MATKEY_SHININESS, shininess)) {
            CO_LOG_WARN("Material {} has no shininess value. Setting default", material->m_name);
            shininess = 0.5;
        }
        CO_LOG_TRACE("Material shininess set to: {}", shininess);

        if (AI_SUCCESS != srcMaterial->Get(AI_MATKEY_OPACITY, opacity)) {
            CO_LOG_WARN("Material {} has no opacity value. Setting default", material->m_name);
            opacity = 1;
        }
        CO_LOG_TRACE("Material opacity set to: {}", opacity);
        // ########################### Getting material colors #########################################################


        // ########################### Converting material colors ######################################################
        auto ambient = convertColor(srcAmbient);
        auto diffuse = convertColor(srcDiffuse);
        auto specular = convertColor(srcSpecular);
        auto emission = convertColor(srcEmission);

        CO_LOG_TRACE("Converting material {} to PBR", material->m_name);

        auto albedo = 0.1f * ambient + 0.6f * diffuse + 0.3f * specular;
        material->m_albedo = albedo;
        material->m_emission = emission;
        material->m_roughness = glm::max(glm::pow(1-shininess/128,2),0.01);
        material->m_transparency = 1-opacity;
        //material->m_metalness = (specular.x/255 + specular.y/255 + specular.z/255)/3;
        material->m_metalness = 0.0f;

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
        bool textureFound = false;
        bool embedded = false;
        aiString diffusePath;
        std::string pathToConvert;

        if(AI_SUCCESS == srcMaterial->Get(AI_MATKEY_TEXTURE(aiTextureType_DIFFUSE, 0), diffusePath)){
            textureFound = true;
            CO_LOG_INFO("Found diffuse texture on material {}", material->m_name);
        }else{
            CO_LOG_TRACE("No diffuse texture found on material {}, trying base color", material->m_name);
        }

        if(!textureFound) {
            if (AI_SUCCESS == srcMaterial->Get(AI_MATKEY_TEXTURE(aiTextureType_BASE_COLOR, 0), diffusePath)) {
                textureFound = true;
                CO_LOG_INFO("Found base color texture on material {}", material->m_name);
            } else {
                CO_LOG_TRACE("No base color texture found on material {}, trying ambient", material->m_name);
            }
        }

        if(!textureFound) {
            if (AI_SUCCESS == srcMaterial->Get(AI_MATKEY_TEXTURE(aiTextureType_AMBIENT, 0), diffusePath)) {
                textureFound = true;
                CO_LOG_INFO("Found ambient texture on material {}", material->m_name);

            } else {
                CO_LOG_TRACE("No ambient texture found on material {}", material->m_name);
            }
        }

        if(!textureFound) CO_LOG_INFO("No textures for material {}", material->m_name);
        else {
            // find and convert texture
            const aiTexture *foundTex = parsingScene->GetEmbeddedTexture(diffusePath.C_Str());
            if (foundTex == nullptr) {
                CO_LOG_TRACE("Texture is external: {}", std::string(diffusePath.C_Str()));
                pathToConvert = diffusePath.C_Str();
            } else {
                CO_LOG_TRACE("Texture is embedded");
                if(foundTex->mHeight==0){
                    CO_LOG_TRACE("Texture is in compressed format, size: {} bytes", foundTex->mWidth);
                    // can write directly to file
                    pathToConvert = std::string("texture_tmp.") + foundTex->achFormatHint;
                    FILE* outTexture = fopen(pathToConvert.c_str(), "wb");
                    fwrite(foundTex->pcData, 1, foundTex->mWidth, outTexture);
                    fclose(outTexture);
                }else{
                    CO_LOG_TRACE("Texture is in uncompressed format, size: {}x{}", foundTex->mWidth, foundTex->mHeight);
                    CO_LOG_TRACE("Using FreeImage to save texture");
                    if(!writeToTGA(foundTex, pathToConvert+"tga")){
                        CO_LOG_ERR("Cannot write temporary texture for conversion. Skipping.");
                        continue;
                    }
                }
                CO_LOG_TRACE("Wrote temporary texture file to {}", pathToConvert);

                //TODO start conversion using img2dds
            }
        }
    }
}