//
// Created by kalu on 3/7/21.
//

#include "coMaterial.h"

const std::string &coMaterial::getAlbedoMap() const {
    return m_albedoMap;
}

void coMaterial::setAlbedoMap(const std::string &albedoMap) {
    m_hasAlbedoMap = true;
    coMaterial::m_albedoMap = albedoMap;
}

const std::string &coMaterial::getNormalMap() const {
    return m_normalMap;
}

void coMaterial::setNormalMap(const std::string &normalMap) {
    m_hasNormalMap = true;
    coMaterial::m_normalMap = normalMap;
}

const std::string &coMaterial::getRoughnessMap() const {
    return m_roughnessMap;
}

void coMaterial::setRoughnessMap(const std::string &roughnessMap) {
    m_hasRoughnessMap = true;
    coMaterial::m_roughnessMap = roughnessMap;
}

const std::string &coMaterial::getMetalnessMap() const {
    return m_metalnessMap;
}

void coMaterial::setMetalnessMap(const std::string &metalnessMap) {
    m_hasMetalnessMap = true;
    coMaterial::m_metalnessMap = metalnessMap;
}

const std::string &coMaterial::getHeightMap() const {
    return m_heightMap;
}

void coMaterial::setHeightMap(const std::string &heightMap) {
    m_hasHeightMap = true;
    coMaterial::m_heightMap = heightMap;
}
