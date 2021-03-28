#pragma once


#include "coNode.h"
#include "coMaterial.h"
#include "map"
#include "coMesh.h"

class coScene {
private:
    std::map<std::string, coMaterial*> m_materials;

    std::vector<coMesh *>m_meshes;

public:
    std::string m_directory;
    coNode* m_rootNode;

    [[nodiscard]] std::map<std::string, coMaterial*> &getMMaterials();

    void setMMaterials(const std::map<std::string, coMaterial*> &mMaterials);

    [[nodiscard]] std::vector<coMesh *> &getMMeshes();

    void setMMeshes(const std::vector<coMesh *> &mMeshes);
};
