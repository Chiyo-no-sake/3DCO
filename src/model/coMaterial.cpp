//
// Created by kalu on 3/7/21.
//

#include "coMaterial.h"
#include <memory.h>

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

char *coMaterial::toChunk(unsigned int *outSize) {

    chunk_header header;
    header.type = getChunkType();

    unsigned int offset = 0;

    // name as cstring + terminator + emissive + albedo + roughness + metalness + transparency
    unsigned int dataBytes = m_name.size() + 1 + 12 + 12 + 4 + 4 + 4;
    dataBytes +=  + m_albedoMap.size() + 1;
    dataBytes +=  + m_normalMap.size() + 1;
    dataBytes +=  + m_roughnessMap.size() + 1;
    dataBytes +=  + m_metalnessMap.size() + 1;

    char *chunk = (char*) malloc(sizeof(chunk_header) + dataBytes);
    char *chunkNoHeader = (char*) malloc(dataBytes);

    strcpy(chunkNoHeader + offset, m_name.c_str());
    offset += m_name.size() + 1;

    memcpy(chunkNoHeader + offset, &m_emission, 12);
    offset += 12;

    memcpy(chunkNoHeader + offset, &m_albedo, 12);
    offset += 12;

    memcpy(chunkNoHeader + offset, &m_roughness, 4);
    offset += 4;

    memcpy(chunkNoHeader + offset, &m_metalness, 4);
    offset += 4;

    memcpy(chunkNoHeader + offset, &m_transparency, 4);
    offset += 4;

    strcpy(chunkNoHeader + offset, m_albedoMap.c_str());
    offset += m_albedoMap.size() + 1;

    strcpy(chunkNoHeader + offset, m_normalMap.c_str());
    offset += m_normalMap.size() + 1;

    strcpy(chunkNoHeader + offset, m_heightMap.c_str());
    offset += m_heightMap.size() + 1;

    strcpy(chunkNoHeader + offset, m_roughnessMap.c_str());
    offset += m_roughnessMap.size() + 1;

    strcpy(chunkNoHeader + offset, m_metalnessMap.c_str());

    header.size = dataBytes;

    memcpy(chunk, &header, sizeof(header));
    memcpy(chunk + sizeof(header), chunkNoHeader, dataBytes);

    delete[] chunkNoHeader;

    *outSize = sizeof(header) + dataBytes;
    return chunk;

}

chunk_type coMaterial::getChunkType() {
    return MATERIAL;
}
