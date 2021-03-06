//
// Created by kalu on 12/25/20.
//

#ifndef CGLIB_PARSINGSTRATEGY_CPP
#define CGLIB_PARSINGSTRATEGY_CPP

#include "ParsingStrategy.h"

void ParsingStrategy::setInputFile(FILE* fp) {
    this->m_fp = fp;
}

void ParsingStrategy::setInputFilePath(const char* path)
{
    this->m_filepath = path;
}

coNode* ParsingStrategy::getResult() const {
    return this->m_parsed;
}

#endif //CGLIB_PARSINGSTRATEGY_CPP
