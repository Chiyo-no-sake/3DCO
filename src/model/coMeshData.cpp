//
// Created by Simone Belingheri on 06/03/2021.
//

#include "coMeshData.h"
#include "glm/gtc/packing.hpp"
#include <memory.h>

glm::vec3 *&coMeshData::getMVertices() {
    return m_vertices;
}

void coMeshData::setMVertices(glm::vec3 *&mVertices) {
    m_vertices = mVertices;
}

unsigned int *&coMeshData::getMIndices() {
    return m_indices;
}

void coMeshData::setMIndices(unsigned int *&mIndices) {
    m_indices = mIndices;
}

glm::vec3 *&coMeshData::getMNormals() {
    return m_normals;
}

void coMeshData::setMNormals(glm::vec3 *&mNormals) {
    m_normals = mNormals;
}

glm::vec3 *&coMeshData::getMTangents() {
    return m_tangents;
}

void coMeshData::setMTangents(glm::vec3 *&mTangents) {
    m_tangents = mTangents;
}

glm::vec3 *&coMeshData::getMBitangents() {
    return m_bitangents;
}

void coMeshData::setMBitangents(glm::vec3 *&mBitangents) {
    m_bitangents = mBitangents;
}

glm::vec3 *&coMeshData::getMTextureCoordinates() {
    return m_textureCoordinates;
}

void coMeshData::setMTextureCoordinates(glm::vec3 *&mTextureCoordinates) {
    m_textureCoordinates = mTextureCoordinates;
}

char *coMeshData::getVerticesDataBuffer(unsigned int *outSize) {
    unsigned int totalSize =
            sizeof(m_numVertices) +
            sizeof(m_numIndices) +
            m_numVertices * (
                    sizeof(glm::vec3) +
                    sizeof(unsigned int) +
                    sizeof(unsigned int) +
                    sizeof(unsigned int));

    char *chunk = (char *) malloc(totalSize);

    unsigned int offset = 0;

    memcpy(chunk+offset, &m_numVertices, sizeof(m_numVertices));
    offset += sizeof(m_numVertices);

    unsigned int facesNum = m_numIndices/3;
    memcpy(chunk+offset, &facesNum, sizeof(facesNum));
    offset+=sizeof(facesNum);

    for (int i = 0; i < m_numVertices; i++) {
        memcpy(chunk + offset, (void *) &m_vertices[i], sizeof(glm::vec3));
        offset += sizeof(glm::vec3);

        unsigned int packedNormal = glm::packSnorm3x10_1x2(glm::vec4{m_normals[i], 0});
        memcpy(chunk + offset, (void *) &packedNormal, sizeof(unsigned int));
        offset += sizeof(unsigned int);

        unsigned int packedUV = glm::packHalf2x16(m_textureCoordinates[i]);
        memcpy(chunk + offset, (void *) &packedUV, sizeof(unsigned int));
        offset += sizeof(unsigned int);

        unsigned int packedTan = glm::packSnorm3x10_1x2(glm::vec4{m_tangents[i], 0});
        memcpy(chunk + offset, (void *) &packedTan, sizeof(unsigned int));
        offset += sizeof(unsigned int);
    }

    *outSize = offset;
    return chunk;
}

char *coMeshData::toChunk(unsigned int *outSize) {
    unsigned int lodDataSize;
    char *lodDataBuffer = getVerticesDataBuffer(&lodDataSize);

    *outSize = lodDataSize + m_numIndices * sizeof(unsigned int);
    char *chunk = (char *) malloc(*outSize);

    memcpy(chunk, lodDataBuffer, lodDataSize);
    memcpy(chunk+lodDataSize, m_indices, m_numIndices*sizeof(unsigned int));

    delete[] lodDataBuffer;
    return chunk;
}




