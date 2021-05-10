//
// Created by Simone Belingheri on 19/03/2021.
//

#include <log/Log.h>
#include <log/VHACDLogger.h>
#include <log/VHACDLogCallback.h>
#include "VHACDGenerator.h"

VHACDGenerator *VHACDGenerator::getInstance() {

    static VHACDGenerator instance;

    return &instance;

}

void VHACDGenerator::init() {

    m_params = VHACD::IVHACD::Parameters{};

    //VHACDLogger logger = VHACDLogger("vhacdlog.txt");
    //VHACDLogCallback callback{};
    //m_params.m_logger = &logger;
    //m_params.m_callback = &callback;

    m_interface = VHACD::CreateVHACD();

}

void VHACDGenerator::compute(coScene *scene) {

    CO_LOG_INFO("Starting convex hull generation");

    for (coMesh *mesh : scene->getMMeshes()) {

        std::vector<coHull *> hulls{};

        if(mesh->m_physicsType == PHYS_DYNAMIC){

            coMeshData *firstLod = mesh->getLODs()[0];
            size_t verticesAsFloat = firstLod->m_numVertices * 3;
            auto *vertices = (float *) malloc(sizeof(float) * verticesAsFloat);

            for (unsigned int i = 0; i < firstLod->m_numVertices; i++) {

                vertices[i * 3] = firstLod->getMVertices()[i].x;
                vertices[i * 3 + 1] = firstLod->getMVertices()[i].y;
                vertices[i * 3 + 2] = firstLod->getMVertices()[i].z;

            }

            CO_LOG_INFO("Computing convex hull for mesh {} ...", mesh->m_name);

            if (!m_interface->Compute(vertices, verticesAsFloat / 3, firstLod->getMIndices(), firstLod->m_numIndices / 3, m_params)) {
                CO_LOG_ERR("Convex Hull generation for mesh {} failed.", mesh->m_name);
            } else {

                VHACD::IVHACD::ConvexHull convexHull{};

                for (unsigned int j = 0; j < m_interface->GetNConvexHulls(); j++) {

                    auto *hull = new coHull();

                    m_interface->GetConvexHull(j, convexHull);

                    CO_LOG_INFO("Convex Hull #{} generated", j);

                    hull->center = {convexHull.m_center[0], convexHull.m_center[1], convexHull.m_center[2]};
                    CO_LOG_TRACE("Hull centroid: {}, {}, {}", hull->center.x, hull->center.y, hull->center.z);

                    auto *hullVertices = (glm::vec3 *)malloc(sizeof(glm::vec3) * convexHull.m_nPoints);
                    auto *hullTriangles = (unsigned int *)malloc(sizeof(unsigned int) * convexHull.m_nTriangles * 3);

                    CO_LOG_TRACE("Hull vertices number: {}", convexHull.m_nPoints);

                    for(unsigned int p = 0; p < convexHull.m_nPoints; p++){

                        hullVertices[p].x = convexHull.m_points[p * 3];
                        hullVertices[p].y = convexHull.m_points[p * 3 + 1];
                        hullVertices[p].z = convexHull.m_points[p * 3 + 2];

                        CO_LOG_TRACE("Vertex {} : {}, {} , {}", p, hullVertices[p].x, hullVertices[p].y, hullVertices[p].z);
                    }

                    CO_LOG_TRACE("Hull triangles number: {}", convexHull.m_nTriangles);

                    for(unsigned int t = 0; t < convexHull.m_nTriangles; t++) {
                        hullTriangles[t*3] = convexHull.m_triangles[t*3];
                        hullTriangles[t*3+1] = convexHull.m_triangles[t*3+1];
                        hullTriangles[t*3+2] = convexHull.m_triangles[t*3+2];
                        CO_LOG_TRACE("Index {}: {}, {}, {}", t, hullTriangles[t*3], hullTriangles[t*3+1], hullTriangles[t*3+2]);
                    }

                    hull->setMHullVertices(hullVertices);
                    hull->setMHullTriangles(hullTriangles);
                    hull->m_numHullTriangles = convexHull.m_nTriangles;
                    hull->m_numHullVertices = convexHull.m_nPoints;

                    hulls.push_back(hull);

                }

                CO_LOG_INFO("Hulls generated for mesh {}", mesh->m_name);
                mesh->setMHulls(hulls);

                delete[] vertices;

            }

        }else CO_LOG_INFO("Mesh {} is not dynamic. No hulls will be generated", mesh->m_name);

    }

    CO_LOG_INFO("Convex hull generation finished");

}

void VHACDGenerator::deinit() {

    m_interface->Clean();
    m_interface->Release();

}
