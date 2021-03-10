//
// Created by Simone Belingheri on 10/03/2021.
//

#include <log/Log.h>
#include "VertexQuantizationStep.h"

void VertexQuantizationStep::execute() {

    CO_LOG_INFO("Initiating vertex quantization step...");
    CO_LOG_INFO("coScene name {}", m_scene->m_rootNode->m_name);
    CO_LOG_INFO("Vertex quantization step completed");

}
