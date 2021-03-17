#include "material_utils.h"

#include <assimp/scene.h>
#include "assimp/texture.h"
#include "log/Log.h"
#include "FreeImage.h"
#include "utils/file_utils.h"

std::string* tryGeDiffusePath(aiMaterial* material){
    aiString diffusePath;
    bool textureFound = false;

    if(AI_SUCCESS == material->Get(AI_MATKEY_TEXTURE(aiTextureType_DIFFUSE, 0), diffusePath)){
        textureFound = true;
        CO_LOG_INFO("Found diffuse texture on material {}", std::string(material->GetName().C_Str()));
    }else{
        CO_LOG_TRACE("No diffuse texture found on material {}, trying base color", std::string(material->GetName().C_Str()));
    }

    if(!textureFound) {
        if (AI_SUCCESS == material->Get(AI_MATKEY_TEXTURE(aiTextureType_BASE_COLOR, 0), diffusePath)) {
            textureFound = true;
            CO_LOG_INFO("Found base color texture on material {}", std::string(material->GetName().C_Str()));
        } else {
            CO_LOG_TRACE("No base color texture found on material {}, trying ambient", std::string(material->GetName().C_Str()));
        }
    }

    if(!textureFound) {
        if (AI_SUCCESS == material->Get(AI_MATKEY_TEXTURE(aiTextureType_AMBIENT, 0), diffusePath)) {
            textureFound = true;
            CO_LOG_INFO("Found ambient texture on material {}", std::string(material->GetName().C_Str()));

        } else {
            CO_LOG_TRACE("No ambient texture found on material {}", std::string(material->GetName().C_Str()));
        }
    }

    return textureFound ? new std::string(diffusePath.C_Str()) : nullptr;
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

std::string convertTexture(const aiScene* parsingScene, const std::string& texturePath, const std::string& materialName,
                    textureType type, const std::string& opacity){
    std::string pathToConvert;
    const aiTexture *foundTex = parsingScene->GetEmbeddedTexture(texturePath.c_str());
    bool external = false;
    bool tmp_used = false;

    if (foundTex == nullptr) {
        CO_LOG_TRACE("Texture is external: {}", texturePath);
        pathToConvert = texturePath;
        external = true;
    } else {
        CO_LOG_TRACE("Texture is embedded");
        if(foundTex->mHeight==0){
            CO_LOG_TRACE("Texture is in compressed format, size: {} bytes", foundTex->mWidth);
            pathToConvert = std::string("texture_tmp.") + foundTex->achFormatHint;
            FILE* outTexture = fopen(pathToConvert.c_str(), "wb");
            if(outTexture == nullptr){
                CO_LOG_ERR("Cannot open file {} for writing tmp texture", pathToConvert);
                return "[none]";
            }
            fwrite(foundTex->pcData, 1, foundTex->mWidth, outTexture);
            fclose(outTexture);
        }else{
            CO_LOG_TRACE("Texture is in uncompressed format, size: {}x{}", foundTex->mWidth, foundTex->mHeight);
            CO_LOG_TRACE("Using FreeImage to save texture");
            if(!writeToTGA(foundTex, pathToConvert+"tga")){
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

    if(!external){
        outPath = materialName + "_diffuse.dds";
    }else {
        outPath.replace(outPath.length() - 3, 3, "dds");
    }
    args << "img2dds" << EXE_POSTFIX;

    if(type == ALBEDO)
        args << " t ";
    else if(type == NORMAL)
        args << " n ";
    else if(type == HEIGHT)
        args << " h ";

    if(opacity != "none")
        args << " -o " << opacity << " ";

    args << pathToConvert << " " << outPath;

    int ret = executeCommand(args.str(), true);
    if(ret == 0){
        CO_LOG_INFO("Texture successfully converted to dds: {}", outPath);
        success = true;
    }else {
        CO_LOG_ERR("Error converting texture with img2dds. Skipping.");
        success = false;
    }

    if(tmp_used){
        if(remove(pathToConvert.c_str()) == 0)
            CO_LOG_TRACE("Deleted tmp file {}", pathToConvert);
        else
            CO_LOG_WARN("Can't delete tmp file {}", pathToConvert);
    }

    return success ? outPath : "[none]";
}

