#pragma once

#include "../model/coNode.h"
#include "ParsingStrategy.h"

class FileParser {
private:
    ParsingStrategy *m_strategy;

public:
    explicit FileParser(ParsingStrategy *strategy = nullptr);

    ~FileParser();

    void setStrategy(ParsingStrategy *strategy);

    coScene *loadFromFile(const std::string &fileName);

};