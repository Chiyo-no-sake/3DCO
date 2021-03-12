#include <log/Log.h>
#include "OVOExporter.h"
#include "utils/files.h"
#include "model/coLight.h"

void writeVersion(FILE* file) {
    chunk_header header;
    header.type = chunk_type::OBJECT;
    header.size = sizeof(unsigned int);
    int currentVersion = 8;
    fwrite(&header, sizeof(header), 1, file);
    fwrite(&currentVersion, sizeof(int), 1, file);
}

void exportChunk(IChunkable* chunkable, FILE* file){
    unsigned int chunkSize;
    char * chunk = chunkable->toChunk(&chunkSize);
    fwrite(chunk, chunkSize, 1, file);
}

void exportNodeRecursive(coNode* node, FILE* file) {
    CO_LOG_TRACE("Exporting node {}", node->m_name);
    exportChunk(node, file);
    CO_LOG_TRACE("Exported node {}", node->m_name);

    for(auto& n: node->getMChildren()){
        exportNodeRecursive(n, file);
    }
}

bool OVOExporter::exportTo(coScene *scene, const char *path) {
    char buffer[FILENAME_MAX];
    GetCurrentDir(buffer, FILENAME_MAX);
    auto finalPath = std::string(buffer) + SEPARATOR + path;

    FILE *file = fopen(finalPath.c_str(), "wb");

    if (file == nullptr) {
        CO_LOG_ERR("Cannot open file {}", finalPath);
        return false;
    }

    CO_LOG_INFO("Exporting scene to {}", finalPath);

    writeVersion(file);

    //export materials

    //export nodes
    exportNodeRecursive(scene->m_rootNode, file);
    fclose(file);
    CO_LOG_INFO("Exported scene to {}", finalPath);
    return true;
}



