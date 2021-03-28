#pragma once

#include <string>
#include <assimp/texture.h>
#include <assimp/material.h>
#include <assimp/scene.h>
#include <glm/vec3.hpp>

enum textureType {
    ALBEDO,
    NORMAL,
    HEIGHT,
    METAL,
    ROUGH
};

bool writeToTGA(const aiTexture* tex, std::string outPath);

std::string* tryGetDiffusePath(aiMaterial* material);
std::string* tryGetNormalPath(aiMaterial* material);
std::string* tryGetMetallicPath(aiMaterial* material);
std::string* tryGetOpacityPath(aiMaterial* material);
std::string* tryGetRoughnessPath(aiMaterial* material);

glm::vec3 getAlbedoColor(aiMaterial* material);
glm::vec3 getEmissionColor(aiMaterial* material);
float getOpacityValue(aiMaterial* material);
float getMetalnessValue(aiMaterial* material);
float getRoughnessValue(aiMaterial* material);

std::string convertTexture(const aiScene* parsingScene, const std::string& texturePath, const std::string& materialName,
                    textureType type,const std::string& opacity = "none");