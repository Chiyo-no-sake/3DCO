#pragma once

enum chunk_type {
    OBJECT = 0, NODE, OBJECT2D, OBJECT3D,
    LIST, BUFFER, SHADER, TEXTURE,
    FILTER, MATERIAL, FBO, QUAD,
    BOX, SKYBOX, FONT, CAMERA,
    LIGHT, BONE, MESH, SKINNED,
    INSTANCED, PIPELINE, EMITTER, ANIM,
    PHYSICS, LAST
};

typedef struct chunk_header_t {
    chunk_type type;
    unsigned int size;
} chunk_header;

class IChunkable {
public:
    /**
     * generate the chunk corresponding to this element
     * @param outSize will be filled with the written size
     * @return the chunk as char buffer
     */
    virtual char* toChunk(unsigned int* outSize) = 0;
};
