#pragma once

#include "coNode.h"
#include "coMeshData.h"
#include "coHull.h"

enum mesh_type {
    DEFAULT = 0, NORMALMAPPED, TESSELLATED, M_UNDEF
};


enum physics_type : char ///< Kind of physical objects
{
    PHYS_UNDEFINED = 0,
    PHYS_DYNAMIC = 1,
    PHYS_KINEMATIC = 2,
    PHYS_STATIC = 3,
    PHYS_LAST
};

enum hull_type : char ///< Kind of hull
{
    HULL_UNDEFINED = 0,
    HULL_SPHERE = 1,
    HULL_BOX = 2,
    HULL_CAPSULE = 3,
    HULL_CONVEX = 4,
    HULL_ORIGINAL = 5,
    HULL_CUSTOM = 6,
    HULL_CONCAVE = 7,
    HULL_LAST
};

class coMesh: public coNode {
public:
    unsigned int m_numLods;
    std::string m_matName;

    float m_mass;
    float m_staticFriction;
    float m_dynamicFriction;
    float m_bounciness;
    float m_linearDamping;
    float m_angularDamping;

    glm::vec3 m_centerOfMass;

    glm::vec3 m_bboxMax;
    glm::vec3 m_bboxMin;

    float m_radius;

    physics_type m_physicsType;

    mesh_type m_meshType;

    hull_type m_hullType;

    [[nodiscard]] std::vector<coMeshData *> &getLODs();

    void setLODs(const std::vector<coMeshData *> &mMeshes);

    [[nodiscard]] const std::vector<coHull *> &getMHulls() const;

    void setMHulls(const std::vector<coHull *> &mHulls);

    chunk_type getChunkType() override;

    char *toChunk(unsigned int *outSize) override;

    [[nodiscard]] glm::vec3 findBoundingBoxMin() const;

    [[nodiscard]] glm::vec3 findBoundingBoxMax() const;

    [[nodiscard]] float computeMeshRadius() const;

private:
    std::vector<coMeshData *> m_LODs{};

    std::vector<coHull *> m_hulls;

    char* getPhysicsDataBuff(unsigned int* outSize) const;
    char* getSkinningDataBuff(unsigned int* outSize) const;
};
