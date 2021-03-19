//
// Created by Simone Belingheri on 18/03/2021.
//

#ifndef INC_3DCO_COHULL_H
#define INC_3DCO_COHULL_H


#include <vector>
#include <glm/glm.hpp>

class coHull {


private:

    std::vector<glm::vec3 *> m_hullVertices;
    std::vector<unsigned int> m_hullTriangles;

public:

    unsigned int m_numHullVertices;
    unsigned int m_numHullTriangles;

    [[nodiscard]] const std::vector<glm::vec3 *> &getMHullVertices() const;

    void setMHullVertices(const std::vector<glm::vec3 *> &mHullVertices);

    [[nodiscard]] const std::vector<unsigned int> &getMHullTriangles() const;

    void setMHullTriangles(const std::vector<unsigned int> &mHullTriangles);

};


#endif //INC_3DCO_COHULL_H
