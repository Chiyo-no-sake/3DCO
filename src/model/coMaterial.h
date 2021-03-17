#pragma once

#include <string>
#include "glm/glm.hpp"

/**
 * Model to save materials
 * TODO:
 *  OVO requires:
 *      Emissive color (rgb)
 *      Albedo color (rgb)
 *      Roughness value (0-1)
 *      Metalness value (0-1)
 *      Transparency value (0-1)
 *      optional data (solo per input pbr):
 *          * albedo map file name --> questa anche per phong
 *          * normal map file name --> " " " "
 *          * roughness map file name
 *          * metalness map file name
 *          * height map file name --> NO
 *
 */
class coMaterial {
public:
    std::string m_name;
    glm::vec3 m_emission;
    glm::vec3 m_albedo;
    float m_roughness;
    float m_metalness;
    float m_transparency;

    [[nodiscard]] const std::string &getAlbedoMap() const;

    void setAlbedoMap(const std::string &albedoMap);

    [[nodiscard]] const std::string &getNormalMap() const;

    void setNormalMap(const std::string &normalMap);

    [[nodiscard]] const std::string &getRoughnessMap() const;

    void setRoughnessMap(const std::string &roughnessMap);

    [[nodiscard]] const std::string &getMetalnessMap() const;

    void setMetalnessMap(const std::string &metalnessMap);

    [[nodiscard]] const std::string &getHeightMap() const;

    void setHeightMap(const std::string &heightMap);

private:
    bool m_hasAlbedoMap = false;
    bool m_hasNormalMap = false;
    bool m_hasRoughnessMap = false;
    bool m_hasMetalnessMap = false;
    bool m_hasHeightMap = false;

    std::string m_albedoMap;
    std::string m_normalMap;
    std::string m_roughnessMap;
    std::string m_metalnessMap;
    std::string m_heightMap;
};
