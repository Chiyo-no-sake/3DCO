//
// Created by Simone Belingheri on 10/03/2021.
//

#include <log/Log.h>
#include "OverdrawOptimizationStep.h"

void OverdrawOptimizationStep::execute() {

    CO_LOG_INFO("Initiating overdraw optimization step...");
    CO_LOG_INFO("coScene name {}", m_scene->m_rootNode->m_name);
    CO_LOG_INFO("Overdraw optimization step completed");

}
