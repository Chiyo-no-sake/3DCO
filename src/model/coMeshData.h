#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <string>
#include <common/IChunkable.h>

class coMeshData: public IChunkable {
private:
    glm::vec3 *m_vertices;
    unsigned int *m_indices;

    //TODO figure out how normals work for points and lines
    glm::vec3 *m_normals;

    glm::vec3 *m_tangents;
    glm::vec3 *m_bitangents;
    glm::vec3 *m_textureCoordinates;
    //    std::vector<coBone *> m_bones;
    //    std::vector<coMaterial *> m_material;

    // TODO Mesh radius size
    // TODO Mesh bounding box minimum corner as a vector of three float components.
    // TODO Mesh bounding box maximum corner as a vector of three float components.

    char * getVerticesDataBuffer(unsigned int* outSize);
public:

    std::string m_name;
    /**
     * The following field gives the length of the vertex, normals, tangents, bitangents and texture coordinates arrays
     */
    unsigned int m_numVertices;

    unsigned int m_numIndices;

    //TODO bones
    //unsigned int m_numBones;

    //TODO materials
    std::string m_materialName;

    [[nodiscard]] glm::vec3 *&getMVertices();

    void setMVertices(glm::vec3 *&mVertices);

    [[nodiscard]] unsigned int*&getMIndices();

    void setMIndices(unsigned int*&mIndices);

    [[nodiscard]] glm::vec3 *&getMNormals();

    void setMNormals(glm::vec3 *&mNormals);

    [[nodiscard]] glm::vec3 *&getMTangents();

    void setMTangents(glm::vec3 *&mTangents);

    [[nodiscard]] glm::vec3 *&getMBitangents();

    void setMBitangents(glm::vec3 *&mBitangents);

    [[nodiscard]] glm::vec3 *&getMTextureCoordinates();

    void setMTextureCoordinates(glm::vec3 *&mTextureCoordinates);

    char * toChunk(unsigned int *outSize) override;
};
