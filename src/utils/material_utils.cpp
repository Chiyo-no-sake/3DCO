#include "material_utils.h"

#include <assimp/scene.h>
#include <assimp/pbrmaterial.h>
#include <sstream>
#include "assimp/texture.h"
#include "log/Log.h"
#include "FreeImage.h"
#include "utils/file_utils.h"

std::string *tryGetDiffusePath(aiMaterial *material) {
    aiString diffusePath;
    bool textureFound = false;

    if (AI_SUCCESS == material->Get(AI_MATKEY_TEXTURE(aiTextureType_DIFFUSE, 0), diffusePath)) {
        textureFound = true;
        CO_LOG_INFO("Found diffuse texture on material {}", std::string(material->GetName().C_Str()));
    } else {
        CO_LOG_TRACE("No diffuse texture found on material {}, trying base color",
                     std::string(material->GetName().C_Str()));
    }

    if (!textureFound) {
        if (AI_SUCCESS == material->Get(AI_MATKEY_TEXTURE(aiTextureType_BASE_COLOR, 0), diffusePath)) {
            textureFound = true;
            CO_LOG_INFO("Found base color texture on material {}", std::string(material->GetName().C_Str()));
        } else {
            CO_LOG_TRACE("No base color texture found on material {}, trying ambient",
                         std::string(material->GetName().C_Str()));
        }
    }

    if (!textureFound) {
        if (AI_SUCCESS == material->Get(AI_MATKEY_TEXTURE(aiTextureType_AMBIENT, 0), diffusePath)) {
            textureFound = true;
            CO_LOG_INFO("Found ambient texture on material {}", std::string(material->GetName().C_Str()));

        } else {
            CO_LOG_TRACE("No ambient texture found on material {}", std::string(material->GetName().C_Str()));
        }
    }

    return textureFound ? new std::string(diffusePath.C_Str()) : nullptr;
}

std::string *tryGetNormalPath(aiMaterial *material) {
    aiString normalPath;
    bool textureFound = false;

    if (AI_SUCCESS == material->Get(AI_MATKEY_TEXTURE(aiTextureType_NORMALS, 0), normalPath)) {
        textureFound = true;
        CO_LOG_INFO("Found normal texture on material {}", std::string(material->GetName().C_Str()));
    } else {
        CO_LOG_TRACE("No normal texture found on material {}", std::string(material->GetName().C_Str()));
    }
    return textureFound ? new std::string(normalPath.C_Str()) : nullptr;
}

std::string *tryGetMetallicPath(aiMaterial *material) {
    aiString metallicPath;
    bool textureFound = false;

    if (AI_SUCCESS == material->GetTexture(AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_METALLICROUGHNESS_TEXTURE, &metallicPath)) {
        textureFound = true;
        CO_LOG_INFO("Found metalness texture on material {}", std::string(material->GetName().C_Str()));
    } else {
        CO_LOG_TRACE("No metalness texture found on material {}", std::string(material->GetName().C_Str()));
    }
    return textureFound ? new std::string(metallicPath.C_Str()) : nullptr;
}

std::string *tryGetRoughnessPath(aiMaterial *material) {
    aiString roughnessPath;
    bool textureFound = false;

    if (AI_SUCCESS == material->GetTexture(AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_METALLICROUGHNESS_TEXTURE, &roughnessPath)) {
        textureFound = true;
        CO_LOG_INFO("Found roughness texture on material {}", std::string(material->GetName().C_Str()));
    } else {
        CO_LOG_TRACE("No roughness texture found on material {}", std::string(material->GetName().C_Str()));
    }
    return textureFound ? new std::string(roughnessPath.C_Str()) : nullptr;
}

std::string *tryGetOpacityPath(aiMaterial *material) {
    aiString opacityPath;
    bool textureFound = false;

    if (AI_SUCCESS == material->Get(AI_MATKEY_TEXTURE(aiTextureType_OPACITY, 0), opacityPath)) {
        textureFound = true;
        CO_LOG_INFO("Found opacity texture on material {}", std::string(material->GetName().C_Str()));
    } else {
        CO_LOG_TRACE("No opacity texture found on material {}", std::string(material->GetName().C_Str()));
    }
    return textureFound ? new std::string(opacityPath.C_Str()) : nullptr;
}

glm::vec3 getAlbedoColor(aiMaterial *material) {
    aiColor3D color;

    if (AI_SUCCESS != material->Get(AI_MATKEY_COLOR_DIFFUSE, color)) {
        CO_LOG_WARN("Material {} has no albedo color. Setting default", std::string(material->GetName().C_Str()));
        color = {.3, .3, .3};
    }
    CO_LOG_TRACE("Material diffuse set to: {}, {}, {}", color.r, color.g, color.b);
    return {color.r, color.g, color.b};
}

glm::vec3 getEmissionColor(aiMaterial *material) {
    aiColor3D color;

    if (AI_SUCCESS != material->Get(AI_MATKEY_COLOR_EMISSIVE, color)) {
        CO_LOG_WARN("Material {} has no emissive color. Setting default", std::string(material->GetName().C_Str()));
        color = {0, 0, 0};
    }
    CO_LOG_TRACE("Material emission set to: {}, {}, {}", color.r, color.g, color.b);
    return {color.r, color.g, color.b};
}

float getOpacityValue(aiMaterial *material) {
    float value;

    if (AI_SUCCESS != material->Get(AI_MATKEY_OPACITY, value)) {
        CO_LOG_WARN("Material {} has no opacity value. Setting default", std::string(material->GetName().C_Str()));
        value = 1;
    }

    CO_LOG_TRACE("Material opacity set to: {}", value);
    return value;
}

float getMetalnessValue(aiMaterial *material) {
    float value;

    if (AI_SUCCESS != material->Get(AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_METALLIC_FACTOR, value)) {
        CO_LOG_WARN("Material {} has no metalness value. Setting default", std::string(material->GetName().C_Str()));
        value = 0;
    }

    CO_LOG_TRACE("Material metalness set to: {}", value);
    return value;
}

float getRoughnessValue(aiMaterial *material) {
    float value;

    if (AI_SUCCESS != material->Get(AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_ROUGHNESS_FACTOR, value)) {
        CO_LOG_WARN("Material {} has no roughness value. Converting from phong",
                    std::string(material->GetName().C_Str()));
        float shine;
        if (AI_SUCCESS != material->Get(AI_MATKEY_SHININESS, shine)) {
            CO_LOG_WARN("Material {} has no shininess value. Setting default roughness",
                        std::string(material->GetName().C_Str()));
            value = 0.001;
        }
        value = glm::max(glm::pow(1 - shine / 128, 2), 0.01);
    }

    CO_LOG_TRACE("Material roughness set to: {}", value);
    return value;
}

bool writeToTGA(const aiTexture *tex, std::string outPath) {
    FreeImage_Initialise();

    FIBITMAP *bitmap = FreeImage_Allocate(tex->mWidth, tex->mHeight, 32);

    for (int y = 0; y < tex->mHeight; y++) {
        for (int x = 0; x < tex->mWidth; x++) {
            aiTexel texel = tex->pcData[x + y];
            RGBQUAD color;
            color.rgbReserved = texel.a;
            color.rgbRed = texel.r;
            color.rgbGreen = texel.g;
            color.rgbBlue = texel.b;

            bool result = FreeImage_SetPixelColor(bitmap, x, y, &color);
            if (!result) return false;
        }
    }

    bool result = FreeImage_Save(FIF_TARGA, bitmap, outPath.c_str(), TARGA_DEFAULT);

    FreeImage_DeInitialise();
    return result;
}

std::string convertTexture(const aiScene *parsingScene, const std::string &texturePath, const std::string &materialName,
                           textureType type, const std::string &opacity) {
    std::string pathToConvert;
    const aiTexture *foundTex = parsingScene->GetEmbeddedTexture(texturePath.c_str());
    bool external = false;
    bool tmp_used = false;

    if (foundTex == nullptr) {
        CO_LOG_TRACE("Texture is external: {}", texturePath);
        pathToConvert = texturePath;
        // attacca prima di texturePath il percorso a parsing scene fino all'ultimo /;
        // parsingScene = ../../tests/assets/simpleFPXScene.fbx --> ../../tests/assets/{texturePath};
        // pathToConvert = directory + texturePath;
        external = true;
    } else {
        CO_LOG_TRACE("Texture is embedded");
        if (foundTex->mHeight == 0) {

            CO_LOG_TRACE("Texture is in compressed format, size: {} bytes", foundTex->mWidth);

            pathToConvert = std::string("texture_tmp.") + foundTex->achFormatHint;
            FILE *outTexture = fopen(pathToConvert.c_str(), "wb");

            if (outTexture == nullptr) {
                CO_LOG_ERR("Cannot open file {} for writing tmp texture", pathToConvert);
                return "[none]";
            }
            fwrite(foundTex->pcData, 1, foundTex->mWidth, outTexture);
            fclose(outTexture);
        } else {

            CO_LOG_TRACE("Texture is in uncompressed format, size: {}x{}", foundTex->mWidth, foundTex->mHeight);
            CO_LOG_TRACE("Using FreeImage to save texture");

            if (!writeToTGA(foundTex, pathToConvert + "tga")) {
                CO_LOG_ERR("Cannot write temporary texture for conversion. Skipping.");
                return "[none]";
            }

        }

        CO_LOG_TRACE("Wrote temporary texture file to {}", pathToConvert);
        tmp_used = true;
    }

    CO_LOG_INFO("Initiating texture conversion with img2dds");
    bool success = true;

    std::stringstream args{};
    std::string outPath = pathToConvert;

    std::string typePostfix;

    switch (type) {
        case ALBEDO:
            typePostfix = "_albedo";
            break;
        case NORMAL:
            typePostfix = "_normal";
            break;
        case METAL:
            typePostfix = "_metal";
            break;
        case HEIGHT:
            typePostfix = "_height";
            break;
        case ROUGH:
            typePostfix = "_rough";
            break;
    }

    if (!external) {
        outPath = materialName + typePostfix + ".dds";
    } else {
        outPath.replace(outPath.length() - 3, 3, "dds");
    }
    args << "img2dds" << EXE_POSTFIX;

    // TODO chiedi al prof come metalness e roughness devono essere convertite in dds
    if (type == ALBEDO) // || METAL || ROUGH
        args << " t ";
    else if (type == NORMAL)
        args << " n ";
    else if (type == HEIGHT || type == METAL || type == ROUGH)
        args << " h ";

    if (opacity != "none")
        args << " -o " << opacity << " ";

    args << pathToConvert << " " << outPath;

    CO_LOG_TRACE("Calling with: {}", args.str());

    int ret = executeCommand(args.str(), true);
    if (ret == 0) {
        CO_LOG_INFO("Texture successfully converted to dds: {}", outPath);
        success = true;
    } else {
        CO_LOG_ERR("Error converting texture with img2dds. Skipping.");
        success = false;
    }

    if (tmp_used) {
        if (remove(pathToConvert.c_str()) == 0)
            CO_LOG_TRACE("Deleted tmp file {}", pathToConvert);
        else
            CO_LOG_WARN("Can't delete tmp file {}", pathToConvert);
    }

    return success ? outPath : "[none]";
}


