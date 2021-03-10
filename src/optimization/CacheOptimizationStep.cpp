//
// Created by Simone Belingheri on 10/03/2021.
//

#include <log/Log.h>
#include "CacheOptimizationStep.h"

void CacheOptimizationStep::execute() {

    CO_LOG_INFO("Initiating cache optimization step...");
    CO_LOG_INFO("coScene name {}", m_scene->m_rootNode->m_name);
    CO_LOG_INFO("Cache optimization step completed");

}
