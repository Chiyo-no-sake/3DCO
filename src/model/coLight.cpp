#include "coLight.h"
#include "memory.h"

typedef struct light_data_t {
     char type;
     glm::vec3 color;
     float influence;
     glm::vec3 direction;
     float cutoff;
     float spotExponent;
     char shadowCast;
     char volumetricLight;
} light_data;

chunk_type coLight::getType() {
    return LIGHT;
}

char *coLight::toChunk(unsigned int *outSize) {
    chunk_header header;
    header.type = getType();

    unsigned int nodeDataSize;
    char* nodeDataBuffer = getNodeChunkData(&nodeDataSize);

    light_data lightData;
    lightData.volumetricLight = m_volumetricLighting;
    lightData.shadowCast = m_shadowCasting;
    lightData.type = m_type;
    lightData.direction = m_direction;
    lightData.cutoff = m_cutoff;
    lightData.color = m_diffuse;
    lightData.spotExponent = m_spotExponent;
    //TODO
    lightData.influence = 0;

    int totalSize = sizeof(chunk_header)+nodeDataSize+sizeof(lightData);
    char* chunk = (char*)malloc(totalSize);
    header.size = nodeDataSize+sizeof(lightData);
    *outSize = totalSize;

    unsigned int offset = 0;
    memcpy(chunk, (void*)&header, sizeof(header));
    offset+=sizeof(header);

    memcpy(chunk+offset, nodeDataBuffer,nodeDataSize);
    offset+=nodeDataSize;

    memcpy(chunk+offset, (void*)&lightData, sizeof(lightData));

    delete [] nodeDataBuffer;
    return chunk;
}
