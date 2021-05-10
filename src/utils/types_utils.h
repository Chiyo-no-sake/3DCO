#pragma once

inline glm::vec3 convertColor(const aiColor3D &c) {
    return glm::vec3{c.r, c.g, c.b};
}

inline glm::vec3 convertVec3(const aiVector3D& v) {
    return glm::vec3{v.x, v.y, v.z};
}

inline glm::mat4 convertMatrix(const aiMatrix4x4 &aiMat) {
    return {
            aiMat.a1, aiMat.b1, aiMat.c1, aiMat.d1,
            aiMat.a2, aiMat.b2, aiMat.c2, aiMat.d2,
            aiMat.a3, aiMat.b3, aiMat.c3, aiMat.d3,
            aiMat.a4, aiMat.b4, aiMat.c4, aiMat.d4
    };
}
