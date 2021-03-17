#pragma once

#include <string>
#include <assimp/texture.h>
#include <assimp/material.h>
#include <assimp/scene.h>

enum textureType {
    ALBEDO,
    NORMAL,
    HEIGHT,
};

bool writeToTGA(const aiTexture* tex, std::string outPath);

std::string* tryGeDiffusePath(aiMaterial* material);

std::string convertTexture(const aiScene* parsingScene, const std::string& texturePath, const std::string& materialName,
                    textureType type, const std::string& opacity = "none");