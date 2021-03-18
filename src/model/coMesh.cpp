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

char *coMesh::getPhysicsDataBuff(unsigned int *outSize) const {
    //TODO
    *outSize = 0;
    return nullptr;
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
    // TODO maybe change
    char meshType = meshType::DEFAULT;
    // TODO include material name
    std::string matName = "[none]";

    float meshRadius = computeMeshRadius();
    glm::vec3 meshBBoxMin = findBoundingBoxMin();
    glm::vec3 meshBBoxMax = findBoundingBoxMax();
    unsigned char physicsIncluded = 0;

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
                  matName.size() + 1 + sizeof(meshRadius) + sizeof(meshBBoxMax) + sizeof(meshBBoxMin) +
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

    memcpy(chunk + offset, &meshRadius, sizeof(meshRadius));
    offset += sizeof(meshRadius);

    memcpy(chunk + offset, &meshBBoxMin, sizeof(meshBBoxMin));
    offset += sizeof(meshBBoxMin);

    memcpy(chunk + offset, &meshBBoxMax, sizeof(meshBBoxMax));
    offset += sizeof(meshBBoxMax);

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
