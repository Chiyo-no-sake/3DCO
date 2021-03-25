#include "coLight.h"
#include "memory.h"

// intensity of light inside phong model at which the light applied in OVO will result 0
float threshold = 0.001;

chunk_type coLight::getChunkType() {
    return LIGHT;
}

static float calculateInfluenceRadius(float constant, float linear, float quadratic) {
    //TODO max float causes problems with ovoviewer
    if(linear == 0 && quadratic == 0){
        return 1.0f/threshold;
    }

    float d1 = (-linear + glm::sqrt(glm::pow(linear,2) - 4*quadratic*(constant-1/threshold)))/2*quadratic;
    float d2 = (-linear - glm::sqrt(glm::pow(linear,2) - 4*quadratic*(constant-1/threshold)))/2*quadratic;
    return glm::max(d1,d2);
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

    float influence = calculateInfluenceRadius(m_constantAttenuation, m_linearAttenuation, m_quadraticAttenuation);
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
