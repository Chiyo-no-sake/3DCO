#include <log/Log.h>
#include "OVOExporter.h"
#include "utils/files.h"
#include "string.h"

struct ovoMaterial {
    //TODO
};

struct ovoMesh {
    //TODO
};

struct ovoLight {
    //TODO
};

struct ovoNode {
    //TODO
};

struct ovoBone {
    //TODO
};

struct fileVersion {
    unsigned int version;
};

struct chunkHeader {
    unsigned int chunkType;
    unsigned int size;
};

void recursiveExport(coNode* node, FILE* file){
    auto* nodeBuffer = (char*)malloc(node->m_name.size()+1);
    strcpy(nodeBuffer, node->m_name.c_str());

}

bool OVOExporter::exportTo(coScene *scene, char *path) {
    char buffer[FILENAME_MAX];
    GetCurrentDir(buffer, FILENAME_MAX);
    auto finalPath = std::string(buffer) + SEPARATOR + path;

    FILE * file = fopen(finalPath.c_str(), "wb");

    if(file == nullptr){
        CO_LOG_ERR("Cannot open file {}", finalPath);
        return false;
    }

    //export materials


    //export nodes
    recursiveExport(scene->m_rootNode, file);
}



