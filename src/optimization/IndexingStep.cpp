//
// Created by Simone Belingheri on 10/03/2021.
//

#include <log/Log.h>
#include "IndexingStep.h"

void IndexingStep::execute() {

    CO_LOG_INFO("Initiating indexing step...");
    CO_LOG_INFO("coScene name {}", m_scene->m_rootNode->m_name);
    CO_LOG_INFO("Indexing step completed");

}
