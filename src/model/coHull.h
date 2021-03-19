//
// Created by Simone Belingheri on 18/03/2021.
//

#ifndef INC_3DCO_COHULL_H
#define INC_3DCO_COHULL_H


#include <vector>
#include <glm/glm.hpp>

class coHull {
private:

    glm::vec3 *m_hullVertices;
    unsigned int *m_hullTriangles;

public:

    unsigned int m_numHullVertices;
    unsigned int m_numHullTriangles;

    glm::vec3 *getMHullVertices() const;

    void setMHullVertices(glm::vec3 *mHullVertices);

    unsigned int *getMHullTriangles() const;

    void setMHullTriangles(unsigned int *mHullTriangles);

};


#endif //INC_3DCO_COHULL_H
