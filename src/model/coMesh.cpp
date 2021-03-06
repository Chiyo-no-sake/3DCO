//
// Created by Simone Belingheri on 06/03/2021.
//

#include "coMesh.h"

const std::vector<glm::vec3> &coMesh::getMVertices() const {
    return m_vertices;
}

void coMesh::setMVertices(const std::vector<glm::vec3> &mVertices) {
    m_vertices = mVertices;
}

const std::vector<unsigned int> &coMesh::getMIndices() const {
    return m_indices;
}

void coMesh::setMIndices(const std::vector<unsigned int> &mIndices) {
    m_indices = mIndices;
}

const std::vector<glm::vec3> &coMesh::getMNormals() const {
    return m_normals;
}

void coMesh::setMNormals(const std::vector<glm::vec3> &mNormals) {
    m_normals = mNormals;
}

const std::vector<glm::vec3> &coMesh::getMTangents() const {
    return m_tangents;
}

void coMesh::setMTangents(const std::vector<glm::vec3> &mTangents) {
    m_tangents = mTangents;
}

const std::vector<glm::vec3> &coMesh::getMBitangents() const {
    return m_bitangents;
}

void coMesh::setMBitangents(const std::vector<glm::vec3> &mBitangents) {
    m_bitangents = mBitangents;
}

const std::vector<glm::vec3> &coMesh::getMTextureCoordinates() const {
    return m_textureCoordinates;
}

void coMesh::setMTextureCoordinates(const std::vector<glm::vec3> &mTextureCoordinates) {
    m_textureCoordinates = mTextureCoordinates;
}

void coMesh::addVertex(glm::vec3 vertex) {
    m_vertices.push_back(vertex);
}

void coMesh::addFace(unsigned int faceVertex) {
    m_indices.push_back(faceVertex);
}

void coMesh::addNormal(glm::vec3 normal) {
    m_normals.push_back(normal);
}

void coMesh::addTangent(glm::vec3 tangent) {
    m_tangents.push_back(tangent);
}

void coMesh::addBitangent(glm::vec3 bitangent) {
    m_bitangents.push_back(bitangent);
}

void coMesh::addTextureCoordinate(glm::vec3 coordinate) {
    m_textureCoordinates.push_back(coordinate);
}

const std::vector<std::vector<glm::vec4>> &coMesh::getMColorSets() const {
    return m_colorSets;
}

void coMesh::setMColorSets(const std::vector<std::vector<glm::vec4>> &mColorSets) {
    m_colorSets = mColorSets;
}



