//
// Created by Simone Belingheri on 10/03/2021.
//

#include <log/Log.h>
#include "CacheOptimizationStep.h"
#include <meshoptimizer.h>

void CacheOptimizationStep::execute() {

//    coMesh *mesh;
//    for(unsigned int i = 0; i < m_scene->m_rootNode->m_numLods; i++)
//        mesh = m_scene->m_rootNode->getMMeshes()[i];

    CO_LOG_INFO("Initiating cache optimization step...");

    //meshopt_optimizeVertexCache(mesh->getMIndices(), mesh->getMIndices(), mesh->m_numIndices,mesh->m_numVertices);

    CO_LOG_INFO("Cache optimization step completed");

}
