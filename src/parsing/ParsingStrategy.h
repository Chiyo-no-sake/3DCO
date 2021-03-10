#pragma once

#include <iostream>
#include <model/coScene.h>
#include <model/coNode.h>
#include <model/coMesh.h>
#include "../common/IStrategy.h"

class ParsingStrategy : public IStrategy {
protected:
    FILE* m_fp;
    const char* m_filepath;
    coScene* m_parsed;
public:
    virtual void execute() = 0;

    void setInputFile(FILE* fp);
    void setInputFilePath(const char* path);

    coScene* getResult() const;
};