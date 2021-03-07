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
