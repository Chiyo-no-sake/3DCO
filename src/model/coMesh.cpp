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

chunk_type coMesh::getType() {
    return MESH;
}

char *coMesh::getPhysicsDataBuff(unsigned int *outSize) {
    //TODO
    *outSize = 0;
    return nullptr;
}

char *coMesh::getSkinningDataBuff(unsigned int *outSize) {
    //TODO
    *outSize = 0;
    return nullptr;
}

char *coMesh::toChunk(unsigned int *outSize) {
    chunk_header header;
    header.type = getType();

    unsigned int nodeDataSize;
    char *nodeDataBuffer = getNodeChunkData(&nodeDataSize);

    // ----------------- mesh data calculation --------------------------
    // TODO maybe change
    char meshType = meshType::DEFAULT;
    // TODO include material name
    std::string matName = "[none]";

    // TODO change this values
    float meshRadius = 30;
    glm::vec3 meshBBoxMin{1.0f, 0.0f, 1.0f};
    glm::vec3 meshBBoxMax{0.0f, 0.0f, 1.0f};
    unsigned char physicsIncluded = 0;

    //physics properties
    unsigned int physicsDataSize;
    char *physicsDataBuf = getPhysicsDataBuff(&physicsDataSize);

    unsigned int lodsNum = m_numLods;

    // array of chunks
    char *lodsChunks[lodsNum];
    unsigned int lodsChunkSizes[lodsNum];
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
    offset += matName.size()+1;

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
