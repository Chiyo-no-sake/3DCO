//
// Created by Simone Belingheri on 06/03/2021.
//

#include "coNode.h"

bool coNode::hasLights() {
    return !m_lights.empty();
}

std::vector<coLight *> coNode::getLights() {
    return m_lights;
}
