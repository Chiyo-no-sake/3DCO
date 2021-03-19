
#include <glm/detail/type_vec3.hpp>
#include "coHull.h"

glm::vec3 *coHull::getMHullVertices() const {
    return m_hullVertices;
}

void coHull::setMHullVertices(glm::vec3 *mHullVertices) {
    m_hullVertices = mHullVertices;
}

unsigned int *coHull::getMHullTriangles() const {
    return m_hullTriangles;
}

void coHull::setMHullTriangles(unsigned int *mHullTriangles) {
    m_hullTriangles = mHullTriangles;
}
