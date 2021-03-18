#include <log/Log.h>
#include "FileParser.h"

#include "utils/file_utils.h"


FileParser::FileParser(ParsingStrategy* strategy) : m_strategy(strategy) {
}

FileParser::~FileParser() {
    if (m_strategy != nullptr) delete m_strategy;
}

void FileParser::setStrategy(ParsingStrategy* strategy) {
    if (m_strategy != nullptr) delete m_strategy;
    this->m_strategy = strategy;
}

coScene* FileParser::loadFromFile(const std::string& fileName) {
    char buffer[FILENAME_MAX];
    GetCurrentDir(buffer, FILENAME_MAX);
    std::string current_dir{buffer};
    current_dir = current_dir + SEPARATOR + fileName;

    CO_LOG_INFO("trying to open file {}", current_dir);
    FILE* in = fopen(current_dir.c_str(), "rb");

    if(in == nullptr){
        CO_LOG_ERR("File not found: {}", current_dir);
        return nullptr;
    }

    m_strategy->setInputFile(in);
    m_strategy->setInputFilePath(fileName.c_str());

    m_strategy->execute();
    return m_strategy->getResult();
}