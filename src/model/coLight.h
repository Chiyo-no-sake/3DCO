#pragma once

#include <string>
#include "glm/glm.hpp"
#include "coNode.h"


enum lightType{
    OMNI = 0, DIRECTIONAL, SPOT, UNDEF
};

class coLight : public coNode {
public:
    lightType m_type;
    glm::vec3 m_position;
    glm::vec3 m_direction;

    glm::vec3 m_ambient;
    glm::vec3 m_diffuse;
    glm::vec3 m_specular;

    float m_linearAttenuation;
    float m_constantAttenuation;
    float m_quadraticAttenuation;
    float m_cutoff;
    float m_spotExponent;
    char m_volumetricLighting;
    char m_shadowCasting;

    chunk_type getType() override;

    static std::string typeToString(lightType l){
        switch(l){
            case DIRECTIONAL:
                return std::string{"DIRECTIONAL"};
            case SPOT:
                return std::string{"SPOT"};
            case OMNI:
                return std::string{"OMNI"};
            case UNDEF:
                return std::string{"UNDEF"};
            default:
                return std::string{""};
        }
    }

private:
    char *toChunk(unsigned int *outSize) override;
};
