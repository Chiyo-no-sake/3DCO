//
// Created by Simone Belingheri on 10/03/2021.
//

#include <log/Log.h>
#include "IndexingStep.h"
#include <meshoptimizer.h>

struct vertexData {
    glm::vec3 vertex;
    glm::vec3 normal;
    glm::vec3 uv;
    glm::vec4 tan;
    glm::vec3 bitan;
};

void IndexingStep::execute() {
    CO_LOG_INFO("Initiating indexing step...");

    for (unsigned int i = 0; i < m_mesh->m_numLods; i++) {

        CO_LOG_TRACE("Indexing LOD {}", i);

        coMeshData *currentLod = m_mesh->getLODs()[i];

        auto* meshVerticesData = new vertexData[currentLod->m_numVertices];

        for(int j=0; j<currentLod->m_numVertices; j++){
            meshVerticesData[j].vertex = currentLod->getMVertices()[j];
            meshVerticesData[j].normal = currentLod->getMNormals()[j];
            meshVerticesData[j].uv = (currentLod->hasTextureCoordinates()) ? currentLod->getMTextureCoordinates()[j] : glm::vec3{0.0f};
            meshVerticesData[j].tan = (currentLod->hasTangents()) ? currentLod->getMTangents()[j] : glm::vec4{0.0f};
            meshVerticesData[j].bitan = (currentLod->hasBitangents()) ? currentLod->getMBitangents()[j] : glm::vec3{0.0f};
        }

        std::vector<unsigned int> remap(currentLod->m_numVertices);

        unsigned int vertexCount = meshopt_generateVertexRemap(&remap[0],
                                                         currentLod->getMIndices(),
                                                         currentLod->m_numIndices,
                                                         meshVerticesData,
                                                         currentLod->m_numVertices,
                                                         sizeof(vertexData));

        auto *newVertices = (vertexData *) malloc(vertexCount * sizeof(vertexData));
        auto *newIndices = (unsigned int *) malloc(currentLod->m_numIndices * sizeof(currentLod->getMIndices()[0]));

        if (newVertices == nullptr || newIndices == nullptr)
            CO_LOG_ERR("Indexing step failed for LoD #{}. Could not generate non-redundant vertex and index buffers", i);

        meshopt_remapVertexBuffer(newVertices,
                                  meshVerticesData,
                                  currentLod->m_numVertices,
                                  sizeof(vertexData),
                                  &remap[0]);

        meshopt_remapIndexBuffer(newIndices,
                                 currentLod->getMIndices(),
                                 currentLod->m_numIndices,
                                 &remap[0]);


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

        delete[] currentLod->getMIndices();
        delete[] meshVerticesData;
        delete[] newVertices;

        currentLod->setMIndices(newIndices);

        CO_LOG_TRACE("New vertex count: {}", vertexCount);
    }


    CO_LOG_INFO("Indexing step completed");

}
