//
// Created by Simone Belingheri on 10/03/2021.
//

#include <log/Log.h>
#include "OverdrawOptimizationStep.h"
#include <meshoptimizer.h>

void OverdrawOptimizationStep::execute() {

    CO_LOG_INFO("Initiating overdraw optimization step...");

    for (unsigned int i = 0; i < m_mesh->m_numLods; i++) {

        coMeshData *currentLod = m_mesh->getLODs()[i];

        meshopt_optimizeOverdraw(currentLod->getMIndices(),
                                 currentLod->getMIndices(),
                                 currentLod->m_numIndices,
                                 &currentLod->getMVertices()[0].x,
                                 currentLod->m_numVertices,
                                 sizeof(currentLod->getMVertices()[0]),
                                 1.05f);

    }

    CO_LOG_INFO("Overdraw optimization step completed");

}
