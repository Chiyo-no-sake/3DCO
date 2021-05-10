#pragma once

float signedTriangleVolume(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3) {
    float v321 = p3.x*p2.y*p1.z;
    float v231 = p2.x*p3.y*p1.z;
    float v312 = p3.x*p1.y*p2.z;
    float v132 = p1.x*p3.y*p2.z;
    float v213 = p2.x*p1.y*p3.z;
    float v123 = p1.x*p2.y*p3.z;
    return (1.0f/6.0f)*(-v321 + v231 + v312 - v132 - v213 + v123);
}

float meshVolume(coMeshData* mesh) {
    float volumesSum = 0;
    auto* meshIndices = mesh->getMIndices();
    auto* meshVertices = mesh->getMVertices();
    for(int i=0; i<mesh->m_numIndices/3; i++) {
        glm::vec3& v1 = meshVertices[meshIndices[i*3]];
        glm::vec3& v2 = meshVertices[meshIndices[i*3+1]];
        glm::vec3& v3 = meshVertices[meshIndices[i*3+2]];

        volumesSum += signedTriangleVolume(v1,v2,v3);
    }
    return glm::abs(volumesSum);
}

glm::vec3 verticesMean(coMeshData* mesh){
    glm::vec3 vSum{0};
    auto* meshVertices = mesh->getMVertices();

    for(int i=0; i<mesh->m_numVertices; i++){
        vSum += meshVertices[i];
    }

    return vSum/(float)mesh->m_numVertices;
}