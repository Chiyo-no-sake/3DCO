//
// Created by Simone Belingheri on 06/03/2021.
//

#ifndef INC_3DCO_CONODE_H
#define INC_3DCO_CONODE_H

#include <glm/glm.hpp>
#include <string>
#include <vector>
#include "coMesh.h"

class coNode {
private:
    std::vector<coMesh *> m_meshes;

    coNode *m_parent;
    std::vector<coNode *> m_children;
public:

    std::string m_name;
    /**
     * Node's transformation matrix relative to the parent
     */
    glm::mat4 m_transform;
    /**
     *  Refers to the number of meshes present in the node. Each mesh represents an LoD instance for the same mesh
     *  It equals 0 if m_meshes has no elements
     */
    unsigned int m_numLods;

    unsigned int m_numChildren;

};


#endif //INC_3DCO_CONODE_H
