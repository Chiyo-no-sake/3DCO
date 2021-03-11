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

char* coMesh::getPhysicsDataBuff(unsigned int* outSize){
    //TODO
    *outSize = 0;
    return nullptr;
}

char* coMesh::getSkinningDataBuff(unsigned int* outSize){
    //TODO
    *outSize = 0;
    return nullptr;
}

char *coMesh::toChunk(unsigned int *outSize) {
    chunk_header header;
    header.type = getType();

    unsigned int nodeDataSize;
    char* nodeDataBuffer = getNodeChunkData(&nodeDataSize);

    // ----------------- mesh data calculation --------------------------
    // TODO maybe change
    char meshType = meshType::DEFAULT;
    // TODO include material name
    std::string matName = "[none]";

    // TODO change this values
    float meshRadius = 0;
    float meshBBoxMax = 0;
    float meshBBoxMin = 0;
    bool physicsIncluded = 0;

    //physics properties
    unsigned int physicsDataSize;
    char* physicsDataBuf = getPhysicsDataBuff(&physicsDataSize);

    unsigned int lodsNum = m_numLods;

    // array of chunks
    char* lodsChunks[lodsNum];
    unsigned int lodsChunkSizes[lodsNum];
    for(int i=0; i<lodsNum; i++){
        lodsChunks[i] = m_LODs[i]->toChunk(&(lodsChunkSizes[i]));
    }

    // skinning properties
    unsigned int skinDataSize;
    char * skinDataBuff = getSkinningDataBuff(&skinDataSize);
    // ------------------------------------------------------------------

    // -------------------- size calculations ---------------------------

    // header + node data + mesh type + matname + terminator + mehsrad + meshbboxmin
    // meshbboxmax + physics flag + physics data + lodsNum
    header.size = nodeDataSize + sizeof(meshType) +
                  matName.size()+1 + sizeof(float)*3 +1 + physicsDataSize +
                  sizeof(unsigned int);

    // + lods data for each lod
    for(int i=0; i<lodsNum;i++){
        header.size += lodsChunkSizes[i];
    }

    // + skinning props
    header.size += skinDataSize;

    int totalSize = header.size + sizeof(chunk_header);
    *outSize = totalSize;
    char *chunk = (char*)malloc(totalSize);
    // ------------------------------------------------------------------

    // ------------------- mesh data writing ----------------------------

    unsigned int offset = 0;
    memcpy(chunk, (void*)&header, sizeof(header));
    offset+=sizeof(header);

    memcpy(chunk+offset, nodeDataBuffer, nodeDataSize);
    offset+=nodeDataSize;

    memcpy(chunk+offset, &meshType, sizeof(meshType));
    offset+=sizeof(meshType);

    strcpy(chunk+offset, matName.c_str());
    offset+=sizeof(matName.size()+1);

    *(chunk+offset) = meshRadius;
    offset+=sizeof(float);

    *(chunk+offset) = meshBBoxMin;
    offset+=sizeof(float);

    *(chunk+offset) = meshBBoxMax;
    offset+=sizeof(float);

    *(chunk+offset) = physicsIncluded;
    offset+=sizeof(physicsIncluded);

    memcpy(chunk+offset, physicsDataBuf, physicsDataSize);
    offset+=physicsDataSize;

    *(chunk+offset) = lodsNum;
    offset+=sizeof(lodsNum);

    for(int i=0; i<lodsNum; i++){
        memcpy(chunk+offset, lodsChunks[i], lodsChunkSizes[i]);
        offset+=lodsChunkSizes[i];
    }

    memcpy(chunk+offset, skinDataBuff, skinDataSize);
    offset+=skinDataSize;

    // ------------------------------------------------------------------

    return chunk;
}
