//
// Created by Simone Belingheri on 10/03/2021.
//

#include <log/Log.h>
#include <meshoptimizer.h>
#include "VertexFetchOptimizationStep.h"

/*TODO:
 * 1) optimizeVertexFetch non sembra accettare glm::vec3 come argomenti
 * 2) nel caso li accetti, non credo che passare lo stesso array funzioni
 *      per via delle modifiche "concorrenti" all'array
*/

void VertexFetchOptimizationStep::execute() {

    CO_LOG_INFO("Initiating vertex fetch optimization step...");

    for (unsigned int i = 0; i < m_mesh->m_numLods; i++) {

        coMeshData *currentLod = m_mesh->getLODs()[i];

        meshopt_optimizeVertexFetch(currentLod->getMVertices(),
                                    currentLod->getMIndices(),
                                    currentLod->m_numIndices,
                                    currentLod->getMVertices(),
                                    currentLod->m_numVertices,
                                    sizeof(currentLod->getMVertices()[0]));

    }

    CO_LOG_INFO("Vertex fetch optimization step completed");

}
