#include <log/Log.h>
#include <filesystem>
#include "FileParser.h"

FileParser::FileParser(ParsingStrategy* strategy) : m_strategy(strategy) {
}

FileParser::~FileParser() {
    if (m_strategy != nullptr) delete m_strategy;
}

void FileParser::setStrategy(ParsingStrategy* strategy) {
    if (m_strategy != nullptr) delete m_strategy;
    this->m_strategy = strategy;
}

coNode* FileParser::loadFromFile(const std::string& fileName) {
    std::string fullPath =  (std::filesystem::current_path() / fileName).string();
    CO_LOG_INFO("trying to open file {}", fullPath);
    FILE* in = fopen(fileName.c_str(), "rb");

    if(in == nullptr){
        CO_LOG_ERR("File not found: {}", fullPath);
        return nullptr;
    }

    m_strategy->setInputFile(in);
    m_strategy->setInputFilePath(fileName.c_str());

    m_strategy->execute();
    return m_strategy->getResult();
}