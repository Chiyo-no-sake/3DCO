//
// Created by Simone Belingheri on 06/03/2021.
//

#include "coNode.h"

<<<<<<< HEAD
bool coNode::hasLights() {
    return !m_lights.empty();
}

std::vector<coLight *> coNode::getLights() {
    return m_lights;
=======
const std::vector<coMesh *> &coNode::getMMeshes() const {
    return m_meshes;
}

void coNode::setMMeshes(const std::vector<coMesh *> &mMeshes) {
    m_meshes = mMeshes;
}

coNode *coNode::getMParent() const {
    return m_parent;
}

void coNode::setMParent(coNode *mParent) {
    m_parent = mParent;
}

const std::vector<coNode *> &coNode::getMChildren() const {
    return m_children;
}

void coNode::setMChildren(const std::vector<coNode *> &mChildren) {
    m_children = mChildren;
}

void coNode::addChild(coNode *child) {
    m_children.push_back(child);
>>>>>>> model-mesh
}
