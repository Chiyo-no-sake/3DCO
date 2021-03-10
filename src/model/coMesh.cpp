//
// Created by kalu on 3/10/21.
//

#include "coMesh.h"

std::vector<coMeshData *> &coMesh::getLODs() {
    return m_LODs;
}

void coMesh::setLODs(const std::vector<coMeshData *> &mMeshes) {
    m_LODs = mMeshes;
}

chunk_type coMesh::getType() {
    return MESH;
}

char *coMesh::toChunk(unsigned int *outSize) {
    return nullptr;
}
