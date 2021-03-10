#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <string>

class coMeshData {
private:
    std::vector<glm::vec3> m_vertices;
    std::vector<unsigned int> m_indices;

    //TODO figure out how normals work for points and lines
    std::vector<glm::vec3> m_normals;

    std::vector<glm::vec3> m_tangents;
    std::vector<glm::vec3> m_bitangents;
    std::vector<glm::vec3> m_textureCoordinates;
    //    std::vector<coBone *> m_bones;
    //    std::vector<coMaterial *> m_material;

    // TODO Mesh radius size
    // TODO Mesh bounding box minimum corner as a vector of three float components.
    // TODO Mesh bounding box maximum corner as a vector of three float components.


public:

    std::string m_name;
    /**
     * The following field gives the length of the vertex, normals, tangents, bitangents and texture coordinates arrays
     */
    unsigned int m_numVertices;

    //TODO bones
    //unsigned int m_numBones;

    //TODO materials
    std::string m_materialName;

    [[nodiscard]] const std::vector<glm::vec3> &getMVertices() const;

    void setMVertices(const std::vector<glm::vec3> &mVertices);

    [[nodiscard]] const std::vector<unsigned int> &getMIndices() const;

    void setMIndices(const std::vector<unsigned int> &mIndices);

    [[nodiscard]] const std::vector<glm::vec3> &getMNormals() const;

    void setMNormals(const std::vector<glm::vec3> &mNormals);

    [[nodiscard]] const std::vector<glm::vec3> &getMTangents() const;

    void setMTangents(const std::vector<glm::vec3> &mTangents);

    [[nodiscard]] const std::vector<glm::vec3> &getMBitangents() const;

    void setMBitangents(const std::vector<glm::vec3> &mBitangents);

    [[nodiscard]] const std::vector<glm::vec3> &getMTextureCoordinates() const;

    void setMTextureCoordinates(const std::vector<glm::vec3> &mTextureCoordinates);

    void addVertex(glm::vec3 vertex);

    void addFace(unsigned int faceVertex);

    void addNormal(glm::vec3 normal);

    void addTangent(glm::vec3 tangent);

    void addBitangent(glm::vec3 bitangent);

    void addTextureCoordinate(glm::vec3 coordinate);
};
