#pragma once

#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <common/IChunkable.h>

class coNode : public IChunkable {
private:
    // PAY ATTENTION:
    //  if this node contains a light that is a spotlight,
    //  and has a child with name <thisname>.TARGET,
    //  that child is the object pointed by the spotlight.
    //  but beware that the transforms necessaries to look to that
    //  object are already applied to this node
    std::vector<coNode *> m_children{};

protected:
    char *getNodeChunkData(unsigned int *outSize);

public:
    std::string m_name;
    /**
     * Node's transformation matrix relative to the parent
     */
    glm::mat4 m_transform;

    unsigned int m_numChildren;

    [[nodiscard]] const std::vector<coNode *> &getMChildren() const;

    void setMChildren(const std::vector<coNode *> &mChildren);

    void addChild(coNode *child);

    coNode *findInChildren(std::string nodeName);

    virtual chunk_type getType();

    char *toChunk(unsigned int *outSize) override;
};

