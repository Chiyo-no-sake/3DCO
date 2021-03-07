#pragma once

/**
 * Model to save materials
 * TODO:
 *  OVO requires:
 *      Emissive color (rgb)
 *      Albedo color (rgb)
 *      Roughness value (0-1)
 *      Metalness value (0-1)
 *      Transparency value (0-1)
 *      optionals data:
 *          * albedo map file name
 *          * normal map file name
 *          * roughness map file name
 *          * metalness map file name
 *          * height map file name
 *
 *      Blinn-Phong to PBR conversion:
 *          ambient = albedo * 0.2
 *          specular = albedo * 0.4
 *          diffuse = albedo * 0.7
 *          shine = (1 - sqrt(rough)) * 128
 */
class coMaterial {

};
