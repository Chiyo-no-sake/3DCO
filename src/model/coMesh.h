//
// Created by kalu on 3/10/21.
//

#ifndef INC_3DCO_COMESH_H
#define INC_3DCO_COMESH_H

#include "coNode.h"
#include "coMeshData.h"

enum meshType {
    DEFAULT = 0, NORMALMAPPED, TESSELLATED, M_UNDEF
};

class coMesh: public coNode {
public:

    unsigned int m_numLods;

    [[nodiscard]] std::vector<coMeshData *> &getLODs();

    void setLODs(const std::vector<coMeshData *> &mMeshes);

    chunk_type getType() override;

    char *toChunk(unsigned int *outSize) override;

private:
    std::vector<coMeshData *> m_LODs{};

    [[nodiscard]] glm::vec3 findBoundingBoxMin() const;
    [[nodiscard]] glm::vec3 findBoundingBoxMax() const;
    [[nodiscard]] float computeMeshRadius() const;
    char* getPhysicsDataBuff(unsigned int* outSize) const;
    char* getSkinningDataBuff(unsigned int* outSize) const;
};


#endif //INC_3DCO_COMESH_H
