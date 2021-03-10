//
// Created by Simone Belingheri on 06/03/2021.
//

#include "coMesh.h"


glm::vec3 *coMesh::getMVertices() const {
    return m_vertices;
}

void coMesh::setMVertices(glm::vec3 *mVertices) {
    m_vertices = mVertices;
}

unsigned int *coMesh::getMIndices() const {
    return m_indices;
}

void coMesh::setMIndices(unsigned int *mIndices) {
    m_indices = mIndices;
}

glm::vec3 *coMesh::getMNormals() const {
    return m_normals;
}

void coMesh::setMNormals(glm::vec3 *mNormals) {
    m_normals = mNormals;
}

glm::vec3 *coMesh::getMTangents() const {
    return m_tangents;
}

void coMesh::setMTangents(glm::vec3 *mTangents) {
    m_tangents = mTangents;
}

glm::vec3 *coMesh::getMBitangents() const {
    return m_bitangents;
}

void coMesh::setMBitangents(glm::vec3 *mBitangents) {
    m_bitangents = mBitangents;
}

glm::vec3 *coMesh::getMTextureCoordinates() const {
    return m_textureCoordinates;
}

void coMesh::setMTextureCoordinates(glm::vec3 *mTextureCoordinates) {
    m_textureCoordinates = mTextureCoordinates;
}
