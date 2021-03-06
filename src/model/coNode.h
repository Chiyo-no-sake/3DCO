#pragma once

#include <glm/glm.hpp>
#include <string>
#include <vector>
#include "coMesh.h"
#include "coLight.h"

class coNode {
private:
    std::vector<coMesh *> m_meshes{};
    std::vector<coLight *> m_lights{};

    coNode *m_parent;
    std::vector<coNode *> m_children{};
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

    bool hasLights();
    std::vector<coLight *>& getMLights();

    std::vector<coMesh *> &getMMeshes();

    void setMMeshes(const std::vector<coMesh *> &mMeshes);

    [[nodiscard]] coNode *getMParent() const;

    void setMParent(coNode *mParent);

    [[nodiscard]] const std::vector<coNode *> &getMChildren() const;

    void setMChildren(const std::vector<coNode *> &mChildren);

    void addChild(coNode *child);

    coNode* findInChildren(std::string nodeName);

};

