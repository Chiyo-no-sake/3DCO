//
// Created by Simone Belingheri on 18/03/2021.
//

#include "coHull.h"

const std::vector<glm::vec3 *> &coHull::getMHullVertices() const {
    return m_hullVertices;
}

void coHull::setMHullVertices(const std::vector<glm::vec3 *> &mHullVertices) {
    m_hullVertices = mHullVertices;
}

const std::vector<unsigned int> &coHull::getMHullTriangles() const {
    return m_hullTriangles;
}

void coHull::setMHullTriangles(const std::vector<unsigned int> &mHullTriangles) {
    m_hullTriangles = mHullTriangles;
}
