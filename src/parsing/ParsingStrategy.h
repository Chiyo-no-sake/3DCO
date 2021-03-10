#pragma once

#include <iostream>
#include <model/coScene.h>
#include "common/IExecutable.h"
#include "../model/coNode.h"

class ParsingStrategy : public IExecutable {
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