#include "ParsingStrategy.h"

void ParsingStrategy::setInputFile(FILE* fp) {
    this->m_fp = fp;
}

void ParsingStrategy::setInputFilePath(const char* path)
{
    this->m_filepath = path;
}

coScene* ParsingStrategy::getResult() const {
    return this->m_parsed;
}

