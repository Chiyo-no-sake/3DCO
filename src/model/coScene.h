#pragma once


#include "coNode.h"
#include "coMaterial.h"
#include "map"

class coScene {
private:
    std::map<std::string, coMaterial*> m_materials;

public:
    coNode* m_rootNode;

    [[nodiscard]] const std::map<std::string, coMaterial*> &getMMaterials() const;

    void setMMaterials(const std::map<std::string, coMaterial*> &mMaterials);
};
