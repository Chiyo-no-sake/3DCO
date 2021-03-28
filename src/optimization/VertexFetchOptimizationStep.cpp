//
// Created by Simone Belingheri on 10/03/2021.
//

#include <log/Log.h>
#include <meshoptimizer.h>
#include "VertexFetchOptimizationStep.h"

struct vertexData {
    glm::vec3 vertex;
    glm::vec3 normal;
    glm::vec3 uv;
    glm::vec3 tan;
    glm::vec3 bitan;
};

void VertexFetchOptimizationStep::execute() {

    CO_LOG_INFO("Initiating vertex fetch optimization step...");

    for (unsigned int i = 0; i < m_mesh->m_numLods; i++) {

        CO_LOG_TRACE("Optimizing vertex fetch on LOD {}", i);

        coMeshData *currentLod = m_mesh->getLODs()[i];

        auto* meshVerticesData = new vertexData[currentLod->m_numVertices];

        for(int j=0; j<currentLod->m_numVertices; j++){
            meshVerticesData[j].vertex = currentLod->getMVertices()[j];
            meshVerticesData[j].normal = currentLod->getMNormals()[j];
            meshVerticesData[j].uv = (currentLod->hasTextureCoordinates()) ? currentLod->getMTextureCoordinates()[j] : glm::vec3{0.0f};
            meshVerticesData[j].tan = (currentLod->hasTangents()) ? currentLod->getMTangents()[j] : glm::vec3{0.0f};
            meshVerticesData[j].bitan = (currentLod->hasBitangents()) ? currentLod->getMBitangents()[j] : glm::vec3{0.0f};
        }

        auto *newVertices = (vertexData *) malloc(sizeof(vertexData) * currentLod->m_numVertices);

        unsigned int vertexCount = meshopt_optimizeVertexFetch(newVertices,
                                                               currentLod->getMIndices(),
                                                               currentLod->m_numIndices,
                                                               meshVerticesData,
                                                               currentLod->m_numVertices,
                                                               sizeof(vertexData));

        currentLod->m_numVertices = vertexCount;
        for(int j=0;j<vertexCount;j++){
            currentLod->getMVertices()[j] = newVertices[j].vertex;
            currentLod->getMNormals()[j] = newVertices[j].normal;

            if(currentLod->hasTangents())
                currentLod->getMTangents()[j] = newVertices[j].tan;

            if(currentLod->hasBitangents())
                currentLod->getMBitangents()[j] = newVertices[j].bitan;

            if(currentLod->hasTextureCoordinates())
                currentLod->getMTextureCoordinates()[j] = newVertices[j].uv;
        }

        delete[] newVertices;
        delete[] meshVerticesData;

        CO_LOG_TRACE("New vertex count: {}", vertexCount);
    }

    CO_LOG_INFO("Vertex fetch optimization step completed");

}
