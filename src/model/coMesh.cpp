//
// Created by kalu on 3/10/21.
//

#include "coMesh.h"
#include <memory.h>

std::vector<coMeshData *> &coMesh::getLODs() {
    return m_LODs;
}

void coMesh::setLODs(const std::vector<coMeshData *> &mMeshes) {
    m_LODs = mMeshes;
}

chunk_type coMesh::getChunkType() {
    return MESH;
}


/*
 * FROM OVOREADER CODE:
 *
 *
 *  struct PhysProps
               {
                  ...
                  float angularDamping;
                  void *physObj;            <--- 8 Byte padding, not 4!!
                  unsigned int nrOfHulls;
                  void *hull;               <--- 8 byte ptr
                                            <--- 8 byte ptr2
                  unsigned int _pad;        <--- NOW 4 byte padding
               };
 */

/**
 * TODO:
     * In OVOREADER the physic part has different paddings than in OVOVIEWER:
     *
     * IN DOCS (WORK WITH OVOVIEWER):
     *      float angularDamping, nrHulls, 4B Padding, 2x VoidPtr
     * OVOREADER only works with:
     *      float angularDamping, voidPtr, nrHulls, 2x voidPtr, 4B Padding
 */

enum : char ///< Kind of physical objects
{
    PHYS_UNDEFINED = 0,
    PHYS_DYNAMIC = 1,
    PHYS_KINEMATIC = 2,
    PHYS_STATIC = 3,
    PHYS_LAST
};

enum : char ///< Kind of hull
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

char *coMesh::getPhysicsDataBuff(unsigned int *outSize) const {
    /*TODO
     * OVOVIEWER parse the file using the order objectType, ccd, crb, hullType.
     * THE DOC SAYS to parse the file using the order objectType, hullType, ccd, crb.
     */

    //TODO x4 which one?
    char objectType = PHYS_DYNAMIC; // ||PHYS_STATIC ? || PHYS_KINEMATIC ?
    char hullType = HULL_CUSTOM;
    char continuousCollDet = 0; // ?
    char collideWithRB = 0; // ?
    const glm::vec3& massCenter = this->m_centerOfMass;
    const float& mass = this->m_mass;
    const float& frictionStat = this->m_staticFriction;
    const float& frictionDyn = this->m_dynamicFriction;
    const float& bounce = this->m_bounciness;
    const float& dampLin = this->m_linearDamping;
    const float& dampAng = this->m_angularDamping;
    // void ptr to pad
    const unsigned int hullsNum = this->m_hulls.size();
    const void* pVoid = nullptr;
    const unsigned int padding = 0;

    // type + hulltype + ccd + crb + massCenter + mass + frictStat
    // + frictDyn + bounce + dampLin + dampAng + hullsNum
    // + pad + ptr1 + ptr2
    unsigned int totalSize = sizeof(objectType) +
                           sizeof(hullType) +
                           sizeof(continuousCollDet) +
                           sizeof(collideWithRB) +
                           sizeof(massCenter) +
                           sizeof(mass) +
                           sizeof(frictionStat) +
                           sizeof(frictionDyn) +
                           sizeof(bounce) +
                           sizeof(dampLin) +
                           sizeof(dampAng) +
                           //ONLY FOR OVOREADER
                           //sizeof(pVoid) +
                           sizeof(hullsNum) +
                           sizeof(pVoid) +
                           sizeof(pVoid) +
                           sizeof(padding);


    // + for each lod:
    for(int i=0; i<hullsNum; i++){
        // number of vertices
        // number of faces
        // hullCenter
        // vertices
        // indices
        totalSize += sizeof(int) + sizeof(int) + sizeof(glm::vec3)
                + sizeof(glm::vec3)*m_hulls[i]->m_numHullVertices
                + sizeof(unsigned int)*m_hulls[i]->m_numHullTriangles*3;
    }

    char * outChunk = new char[totalSize];
    unsigned int offset = 0;

    memcpy(outChunk+offset, &objectType, sizeof(objectType));
    offset += sizeof(objectType);

    memcpy(outChunk+offset, &hullType, sizeof(hullType));
    offset += sizeof(hullType);

    memcpy(outChunk+offset, &continuousCollDet, sizeof(continuousCollDet));
    offset += sizeof(continuousCollDet);

    memcpy(outChunk+offset, &collideWithRB, sizeof(collideWithRB));
    offset += sizeof(collideWithRB);

    memcpy(outChunk+offset, &massCenter, sizeof(massCenter));
    offset += sizeof(massCenter);

    memcpy(outChunk+offset, &mass, sizeof(mass));
    offset += sizeof(mass);

    memcpy(outChunk+offset, &frictionStat, sizeof(frictionStat));
    offset += sizeof(frictionStat);

    memcpy(outChunk+offset, &frictionDyn, sizeof(frictionDyn));
    offset += sizeof(frictionDyn);

    memcpy(outChunk+offset, &bounce, sizeof(bounce));
    offset += sizeof(bounce);

    memcpy(outChunk+offset, &dampLin, sizeof(dampLin));
    offset += sizeof(dampLin);

    memcpy(outChunk+offset, &dampAng, sizeof(dampAng));
    offset += sizeof(dampAng);

    // ONLY FOR OVOREADER
    //memcpy(outChunk+offset, &pVoid, sizeof(pVoid));
    //offset += sizeof(pVoid);

    memcpy(outChunk+offset, &hullsNum, sizeof(hullsNum));
    offset += sizeof(hullsNum);

    memcpy(outChunk+offset, &pVoid, sizeof(pVoid));
    offset += sizeof(pVoid);

    memcpy(outChunk+offset, &pVoid, sizeof(pVoid));
    offset += sizeof(pVoid);

    memcpy(outChunk+offset, &padding, sizeof(padding));
    offset += sizeof(padding);

    for(int i=0; i<hullsNum; i++){
        unsigned int& numVertices = m_hulls[i]->m_numHullVertices;
        unsigned int& numTriangles = m_hulls[i]->m_numHullTriangles;
        glm::vec3& hullCentroid = m_hulls[i]->center;
        auto vertices = m_hulls[i]->getMHullVertices();
        auto triangles = m_hulls[i]->getMHullTriangles();

        memcpy(outChunk+offset, &numVertices, sizeof(numVertices));
        offset += sizeof(numVertices);

        memcpy(outChunk+offset, &numTriangles, sizeof(numTriangles));
        offset += sizeof(numTriangles);

        memcpy(outChunk+offset, &hullCentroid, sizeof(hullCentroid));
        offset += sizeof(hullCentroid);

        memcpy(outChunk+offset, vertices, sizeof(glm::vec3)*numVertices);
        offset += sizeof(glm::vec3)*numVertices;

        memcpy(outChunk+offset, triangles, sizeof(unsigned int)*numTriangles*3);
        offset += sizeof(unsigned int)*numTriangles*3;
    }

    *outSize = totalSize;
    return outChunk;
}

char *coMesh::getSkinningDataBuff(unsigned int *outSize) const {
    //TODO
    *outSize = 0;
    return nullptr;
}

char *coMesh::toChunk(unsigned int *outSize) {
    chunk_header header;
    header.type = getChunkType();

    unsigned int nodeDataSize;
    char *nodeDataBuffer = getNodeChunkData(&nodeDataSize);

    // ----------------- mesh data calculation --------------------------
    // TODO if material has normal map, it is NORMALMAPPED
    std::string matName = m_matName;

    char meshType = meshType::DEFAULT;

    // TODO
    unsigned char physicsIncluded = 1;

    //physics properties
    unsigned int physicsDataSize;
    char *physicsDataBuf = getPhysicsDataBuff(&physicsDataSize);

    unsigned int lodsNum = m_numLods;

    // array of chunks
    char **lodsChunks = (char **) malloc(lodsNum * sizeof(nullptr));
    auto *lodsChunkSizes = new unsigned int[lodsNum];
    for (int i = 0; i < lodsNum; i++) {
        lodsChunks[i] = m_LODs[i]->toChunk(&(lodsChunkSizes[i]));
    }

    // skinning properties
    unsigned int skinDataSize;
    char *skinDataBuff = getSkinningDataBuff(&skinDataSize);
    // ------------------------------------------------------------------

    // -------------------- size calculations ---------------------------

    // node data + mesh type + matname + terminator + mehsrad + meshbboxmin
    // meshbboxmax + physics flag + physics data + lodsNum
    header.size = nodeDataSize + sizeof(meshType) +
                  matName.size() + 1 + sizeof(m_radius) + sizeof(m_bboxMax) + sizeof(m_bboxMin) +
                  sizeof(physicsIncluded) + physicsDataSize +
                  sizeof(unsigned int);

    // + lods data for each lod
    for (int i = 0; i < lodsNum; i++) {
        header.size += lodsChunkSizes[i];
    }

    // + skinning props
    header.size += skinDataSize;

    unsigned int totalSize = header.size + sizeof(chunk_header);
    *outSize = totalSize;
    char *chunk = (char *) malloc(totalSize);
    // ------------------------------------------------------------------

    // ------------------- mesh data writing ----------------------------

    unsigned int offset = 0;
    memcpy(chunk, (void *) &header, sizeof(header));
    offset += sizeof(header);

    memcpy(chunk + offset, nodeDataBuffer, nodeDataSize);
    offset += nodeDataSize;

    memcpy(chunk + offset, &meshType, sizeof(meshType));
    offset += sizeof(meshType);

    strcpy(chunk + offset, matName.c_str());
    offset += matName.size() + 1;

    memcpy(chunk + offset, &m_radius, sizeof(m_radius));
    offset += sizeof(m_radius);

    memcpy(chunk + offset, &m_bboxMin, sizeof(m_bboxMin));
    offset += sizeof(m_bboxMin);

    memcpy(chunk + offset, &m_bboxMax, sizeof(m_bboxMax));
    offset += sizeof(m_bboxMax);

    memcpy(chunk + offset, &physicsIncluded, sizeof(physicsIncluded));
    offset += sizeof(physicsIncluded);

    memcpy(chunk + offset, physicsDataBuf, physicsDataSize);
    offset += physicsDataSize;

    memcpy(chunk + offset, &lodsNum, sizeof(lodsNum));
    offset += sizeof(lodsNum);

    for (int i = 0; i < lodsNum; i++) {
        memcpy(chunk + offset, lodsChunks[i], lodsChunkSizes[i]);
        offset += lodsChunkSizes[i];
    }

    memcpy(chunk + offset, skinDataBuff, skinDataSize);
    offset += skinDataSize;

    // ------------------------------------------------------------------

    for (int i = 0; i < lodsNum; i++)
        delete[] lodsChunks[i];
    delete[] physicsDataBuf;
    delete[] skinDataBuff;
    delete[] nodeDataBuffer;

    return chunk;
}


float coMesh::computeMeshRadius() const {
    float maxDist;

    coMeshData *firstLod = m_LODs[0];
    maxDist = glm::length(firstLod->getMVertices()[0]);

    for (int i = 1; i < firstLod->m_numVertices; i++) {
        float dist = glm::length(firstLod->getMVertices()[i]);
        if (dist > maxDist) {
            maxDist = dist;
        }
    }

    return maxDist;
}

glm::vec3 coMesh::findBoundingBoxMin() const {
    float minX;
    float minY;
    float minZ;

    glm::vec3 *vertices = m_LODs[0]->getMVertices();
    minX = vertices[0].x;
    minY = vertices[0].y;
    minZ = vertices[0].z;

    for (int i = 1; i < m_LODs[0]->m_numVertices; i++) {
        glm::vec3 &v = vertices[i];
        if (v.x < minX) minX = v.x;
        if (v.y < minY) minY = v.y;
        if (v.z < minZ) minZ = v.z;
    }

    return {minX, minY, minZ};
}

glm::vec3 coMesh::findBoundingBoxMax() const {
    float maxX;
    float maxY;
    float maxZ;

    glm::vec3 *vertices = m_LODs[0]->getMVertices();
    maxX = vertices[0].x;
    maxY = vertices[0].y;
    maxZ = vertices[0].z;

    for (int i = 1; i < m_LODs[0]->m_numVertices; i++) {
        glm::vec3 &v = vertices[i];
        if (v.x > maxX) maxX = v.x;
        if (v.y > maxY) maxY = v.y;
        if (v.z > maxZ) maxZ = v.z;
    }

    return {maxX, maxY, maxZ};
}

const std::vector<coHull *> &coMesh::getMHulls() const {
    return m_hulls;
}

void coMesh::setMHulls(const std::vector<coHull *> &mHulls) {
    m_hulls = mHulls;
}
