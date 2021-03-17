#include "coLight.h"
#include "memory.h"

chunk_type coLight::getChunkType() {
    return LIGHT;
}

char *coLight::toChunk(unsigned int *outSize) {
    chunk_header header;
    header.type = getChunkType();

    unsigned int nodeDataSize;
    char *nodeDataBuffer = getNodeChunkData(&nodeDataSize);

    unsigned int lightDataSize =
            sizeof(char) + sizeof(glm::vec3) + sizeof(float) + sizeof(glm::vec3) + sizeof(float) * 2 + sizeof(char) * 2;

    unsigned int totalSize = sizeof(chunk_header) + nodeDataSize + lightDataSize;
    char *chunk = (char *) malloc(totalSize);
    header.size = nodeDataSize + lightDataSize;
    *outSize = totalSize;

    unsigned int offset = 0;
    memcpy(chunk, (void *) &header, sizeof(header));
    offset += sizeof(header);

    memcpy(chunk + offset, nodeDataBuffer, nodeDataSize);
    offset += nodeDataSize;

    char lightType = m_type;
    memcpy(chunk+offset, &lightType, 1);
    offset += 1;

    memcpy(chunk+offset, &m_diffuse, sizeof(m_diffuse));
    offset += sizeof(m_diffuse);

    // TODO calculate influence
    float influence = 0;
    memcpy(chunk+offset, &influence, sizeof(influence));
    offset+=sizeof(influence);

    memcpy(chunk+offset, &m_direction, sizeof(m_direction));
    offset+=sizeof(m_direction);

    memcpy(chunk+offset, &m_cutoff, sizeof(m_cutoff));
    offset+=sizeof(m_cutoff);

    memcpy(chunk+offset, &m_spotExponent, sizeof(m_spotExponent));
    offset+=sizeof(m_spotExponent);

    memcpy(chunk+offset, &m_shadowCasting, sizeof(m_shadowCasting));
    offset+= sizeof(m_shadowCasting);

    memcpy(chunk+offset, &m_volumetricLighting, sizeof(m_volumetricLighting));

    delete[] nodeDataBuffer;
    return chunk;
}
