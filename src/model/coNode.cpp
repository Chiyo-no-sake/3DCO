//
// Created by Simone Belingheri on 06/03/2021.
//

#include "coNode.h"
#include "memory.h"

const std::vector<coNode *> &coNode::getMChildren() const {
    return m_children;
}

void coNode::setMChildren(const std::vector<coNode *> &mChildren) {
    m_children = mChildren;
}

void coNode::addChild(coNode *child) {
    m_children.push_back(child);
}

coNode *coNode::findInChildren(std::string nodeName) {
    if(this->m_name == nodeName) return this;

    for(auto& node: m_children){
        coNode* found = node->findInChildren(nodeName);
        if(found != nullptr) return found;
    }

    return nullptr;
}

chunk_type coNode::getChunkType() {
    return NODE;
}

char *coNode::toChunk(unsigned int *outSize) {
    chunk_header header;
    header.type = this->getChunkType();

    unsigned int nodeDataSize;
    char* nodeBuffer = getNodeChunkData(&nodeDataSize);
    header.size = nodeDataSize;

    char* chunk = (char*) malloc(sizeof(chunk_header)+nodeDataSize);
    memcpy(chunk, (void*)&header, sizeof(header));
    memcpy(chunk+sizeof(header), nodeBuffer, nodeDataSize);

    delete[] nodeBuffer;

    *outSize = sizeof(header) + nodeDataSize;
    return chunk;
}

char *coNode::getNodeChunkData(unsigned int * outSize) {
    // search for target node
    std::string targetName = m_name + ".TARGET";
    coNode* targetNode = findInChildren(targetName);
    if(targetNode == nullptr) targetName = "[none]";

    // name + terminator + transform + numChilds + target + terminator
    unsigned int bytes = m_name.size() + 1 + 64 + 4 + targetName.size() + 1;
    char *outBuff = (char *) malloc(bytes);

    unsigned int offset = 0;

    strcpy(outBuff + offset, m_name.c_str());
    offset += m_name.size() + 1;

    memcpy(outBuff + offset, (void *) &(m_transform), 64);
    offset += 64;

    memcpy(outBuff+offset, &m_numChildren, sizeof(unsigned int));
    offset+=sizeof(unsigned int);

    strcpy(outBuff+offset, targetName.c_str());

    // set out size
    *outSize = bytes;
    return outBuff;
}
