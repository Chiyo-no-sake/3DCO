#pragma once

#include <string>
#include "coColored.h"
#include "glm/glm.hpp"

enum lightType{
    DIRECTIONAL = 0,
    SPOT,
    OMNI,
    UNDEF
};

class coLight : public coColored {
public:
    lightType m_lightType;
    glm::vec3 m_position;
    glm::vec3 m_direction;
    std::string m_name;

    float m_linearAttenuation;
    float m_constantAttenuation;
    float m_quadraticAttenuation;

    float m_innerConeAngle;
    float m_outerConeAngle;

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
};
