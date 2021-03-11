//
// Created by kalu on 3/10/21.
//

#ifndef INC_3DCO_COMESH_H
#define INC_3DCO_COMESH_H

#include "coNode.h"
#include "coMeshData.h"

class coMesh: public coNode {
private:
    std::vector<coMeshData *> m_LODs{};
public:
    int m_numLods;

    [[nodiscard]] std::vector<coMeshData *> &getLODs();

    void setLODs(const std::vector<coMeshData *> &mMeshes);

    chunk_type getType() override;

private:
    char *toChunk(unsigned int *outSize) override;
};


#endif //INC_3DCO_COMESH_H
