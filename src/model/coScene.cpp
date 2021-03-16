//
// Created by kalu on 3/7/21.
//

#include "coScene.h"

const std::map<std::string, coMaterial*> &coScene::getMMaterials() const {
    return m_materials;
}

void coScene::setMMaterials(const std::map<std::string, coMaterial*> &mMaterials) {
    m_materials = mMaterials;
}

std::vector<coMesh *> &coScene::getMMeshes() {
    return m_meshes;
}

void coScene::setMMeshes(const std::vector<coMesh *> &mMeshes) {
    m_meshes = mMeshes;
}
