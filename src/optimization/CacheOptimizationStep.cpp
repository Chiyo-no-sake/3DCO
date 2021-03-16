//
// Created by Simone Belingheri on 10/03/2021.
//

#include <log/Log.h>
#include "CacheOptimizationStep.h"
#include <meshoptimizer.h>

void CacheOptimizationStep::execute() {

    CO_LOG_INFO("Initiating cache optimization step...");

    for (unsigned int i = 0; i < m_mesh->m_numLods; i++) {

        coMeshData *currentLod = m_mesh->getLODs()[i];

        meshopt_optimizeVertexCache(currentLod->getMIndices(),
                                    currentLod->getMIndices(),
                                    currentLod->m_numIndices,
                                    currentLod->m_numVertices);

    }

    CO_LOG_INFO("Cache optimization step completed");

}
