//
// Created by Simone Belingheri on 10/03/2021.
//

#include <log/Log.h>
#include "IndexingStep.h"
#include <meshoptimizer.h>

void IndexingStep::execute() {

    CO_LOG_INFO("Initiating indexing step...");

    for (unsigned int i = 0; i < m_mesh->m_numLods; i++) {

        coMeshData *currentLod = m_mesh->getLODs()[i];
        std::vector<unsigned int> remap(currentLod->m_numIndices);

        size_t vertexCount = meshopt_generateVertexRemap(&remap[0],
                                                         currentLod->getMIndices(),
                                                         currentLod->m_numIndices,
                                                         &currentLod->getMVertices(),
                                                         currentLod->m_numIndices,
                                                         sizeof(currentLod->getMVertices()[0]));

        auto *newVertices = (glm::vec3 *) malloc(vertexCount * sizeof(currentLod->getMVertices()[0]));
        auto *newIndices = (unsigned int *) malloc(currentLod->m_numIndices * sizeof(currentLod->getMIndices()[0]));

        if (newVertices == nullptr || newIndices == nullptr)
            CO_LOG_ERR("Indexing step failed for LoD #{}. Could not generate non-redundant vertex and index buffers", i);

        meshopt_remapVertexBuffer(newVertices,
                                  &currentLod->getMVertices()[0],
                                  currentLod->m_numIndices,
                                  sizeof(currentLod->getMVertices()[0]),
                                  &remap[0]);

        meshopt_remapIndexBuffer(newIndices,
                                 currentLod->getMIndices(),
                                 currentLod->m_numIndices,
                                 &remap[0]);

        delete[] currentLod->getMVertices();
        delete[] currentLod->getMIndices();

        currentLod->setMVertices(newVertices);
        currentLod->setMIndices(newIndices);

    }


    CO_LOG_INFO("Indexing step completed");

}
