//
// Created by Simone Belingheri on 10/03/2021.
//

#include <log/Log.h>
#include "VertexFetchOptimizationStep.h"

void VertexFetchOptimizationStep::execute() {

    CO_LOG_INFO("Initiating vertex fetch optimization step...");
    CO_LOG_INFO("coScene name {}", m_scene->m_rootNode->m_name);
    CO_LOG_INFO("Vertex fetch optimization step completed");

}
