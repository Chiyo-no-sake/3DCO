//
// Created by Simone Belingheri on 06/03/2021.
//

#include "coMeshData.h"

const std::vector<glm::vec3> &coMeshData::getMVertices() const {
    return m_vertices;
}

void coMeshData::setMVertices(const std::vector<glm::vec3> &mVertices) {
    m_vertices = mVertices;
}

const std::vector<unsigned int> &coMeshData::getMIndices() const {
    return m_indices;
}

void coMeshData::setMIndices(const std::vector<unsigned int> &mIndices) {
    m_indices = mIndices;
}

const std::vector<glm::vec3> &coMeshData::getMNormals() const {
    return m_normals;
}

void coMeshData::setMNormals(const std::vector<glm::vec3> &mNormals) {
    m_normals = mNormals;
}

const std::vector<glm::vec3> &coMeshData::getMTangents() const {
    return m_tangents;
}

void coMeshData::setMTangents(const std::vector<glm::vec3> &mTangents) {
    m_tangents = mTangents;
}

const std::vector<glm::vec3> &coMeshData::getMBitangents() const {
    return m_bitangents;
}

void coMeshData::setMBitangents(const std::vector<glm::vec3> &mBitangents) {
    m_bitangents = mBitangents;
}

const std::vector<glm::vec3> &coMeshData::getMTextureCoordinates() const {
    return m_textureCoordinates;
}

void coMeshData::setMTextureCoordinates(const std::vector<glm::vec3> &mTextureCoordinates) {
    m_textureCoordinates = mTextureCoordinates;
}

void coMeshData::addVertex(glm::vec3 vertex) {
    m_vertices.push_back(vertex);
}

void coMeshData::addFace(unsigned int faceVertex) {
    m_indices.push_back(faceVertex);
}

void coMeshData::addNormal(glm::vec3 normal) {
    m_normals.push_back(normal);
}

void coMeshData::addTangent(glm::vec3 tangent) {
    m_tangents.push_back(tangent);
}

void coMeshData::addBitangent(glm::vec3 bitangent) {
    m_bitangents.push_back(bitangent);
}

void coMeshData::addTextureCoordinate(glm::vec3 coordinate) {
    m_textureCoordinates.push_back(coordinate);
}



