#pragma once

#include <iostream>
#include "../common/IStrategy.h"
#include "../model/coNode.h"

class ParsingStrategy : public IStrategy {
protected:
    FILE* m_fp;
    const char* m_filepath;
    coNode* m_parsed;
public:
    virtual void execute() = 0;

    void setInputFile(FILE* fp);
    void setInputFilePath(const char* path);

    coNode* getResult() const;
};