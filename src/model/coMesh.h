//
// Created by Simone Belingheri on 06/03/2021.
//

#ifndef INC_3DCO_COMESH_H
#define INC_3DCO_COMESH_H

#include <vector>
#include <glm/glm.hpp>
#include <string>

class coMesh {
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

public:

    std::string m_name;
    /**
     * The following field gives the length of the vertex, normals, tangents, bitangents and texture coordinates arrays
     */
    unsigned int m_numVertices;

    //TODO bones
    //unsigned int m_numBones;

    //TODO materials
    //unsigned int m_numMaterials;

    const std::vector<glm::vec3> &getMVertices() const;

    void setMVertices(const std::vector<glm::vec3> &mVertices);

    const std::vector<unsigned int> &getMIndices() const;

    void setMIndices(const std::vector<unsigned int> &mIndices);

    const std::vector<glm::vec3> &getMNormals() const;

    void setMNormals(const std::vector<glm::vec3> &mNormals);

    const std::vector<glm::vec3> &getMTangents() const;

    void setMTangents(const std::vector<glm::vec3> &mTangents);

    const std::vector<glm::vec3> &getMBitangents() const;

    void setMBitangents(const std::vector<glm::vec3> &mBitangents);

    const std::vector<glm::vec3> &getMTextureCoordinates() const;

    void setMTextureCoordinates(const std::vector<glm::vec3> &mTextureCoordinates);

    const std::vector<std::vector<glm::vec4>> &getMColorSets() const;

    void setMColorSets(const std::vector<std::vector<glm::vec4>> &mColorSets);

    void addVertex(glm::vec3 vertex);

    void addFace(unsigned int faceVertex);

    void addNormal(glm::vec3 normal);

    void addTangent(glm::vec3 tangent);

    void addBitangent(glm::vec3 bitangent);

    void addTextureCoordinate(glm::vec3 coordinate);



};


#endif //INC_3DCO_COMESH_H
