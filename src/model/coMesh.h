#pragma once

#include "coNode.h"
#include "coMeshData.h"
#include "coHull.h"

enum meshType {
    DEFAULT = 0, NORMALMAPPED, TESSELLATED, M_UNDEF
};

class coMesh: public coNode {
public:
    unsigned int m_numLods;
    std::string m_matName;

    [[nodiscard]] std::vector<coMeshData *> &getLODs();

    void setLODs(const std::vector<coMeshData *> &mMeshes);

    [[nodiscard]] const std::vector<coHull *> &getMHulls() const;

    void setMHulls(const std::vector<coHull *> &mHulls);

    chunk_type getChunkType() override;

    char *toChunk(unsigned int *outSize) override;

private:
    std::vector<coMeshData *> m_LODs{};

    std::vector<coHull *> m_hulls;

    [[nodiscard]] glm::vec3 findBoundingBoxMin() const;
    [[nodiscard]] glm::vec3 findBoundingBoxMax() const;
    [[nodiscard]] float computeMeshRadius() const;
    char* getPhysicsDataBuff(unsigned int* outSize) const;
    char* getSkinningDataBuff(unsigned int* outSize) const;
};
