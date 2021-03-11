//
// Created by Simone Belingheri on 06/03/2021.
//

#include "coMeshData.h"

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




