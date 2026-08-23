#version 460

const vec2 QUAD_UVS[6] = vec2[6](vec2(0.0, 0.0), // 0: Bottom-Left
    vec2(1.0, 0.0),                              // 1: Bottom-Right
    vec2(1.0, 1.0),                              // 2: Top-Right

    vec2(1.0, 1.0), // 3: Top-Right
    vec2(0.0, 1.0), // 4: Top-Left
    vec2(0.0, 0.0)  // 5: Bottom-Left
);

struct Vert {
    vec3 pos;
    float light;
};

struct Face {
    uint face;
    uint texId;
    uint breakStage;
    uint pad;
};

struct Chunk {
    vec3 pos;
    uint faceOffset;
    uint baseVertex;
    uint isUniform;
    uint padding[2];
};

layout(std430, binding = 0) readonly buffer ssbo0 { Chunk chunks[]; };
layout(std430, binding = 1) readonly buffer ssbo1 { Face faces[]; };
layout(std430, binding = 2) readonly buffer ssbo2 { Vert verts[]; };

layout(location = 0) uniform mat4 uVP;

flat out float vFace;
flat out float vTex;
flat out float vLight;
flat out float vBreakStage;
out vec2 vTexCoord;

vec2 GetChunkUV(vec3 pos, uint faceType, uint isUniform, uint baseVertex) {
    if (isUniform == 0u) {
        return QUAD_UVS[(uint(gl_VertexID) - baseVertex) % 6u];
    }

    switch (faceType) {
    case 0u:
        return pos.xy; // Front (+Z)
    case 1u:
        return vec2(16.0 - pos.x, pos.y); // Back (-Z)
    case 2u:
        return pos.xz; // Top (+Y)
    case 3u:
        return vec2(pos.x, 16.0 - pos.z); // Bottom (-Y)
    case 4u:
        return vec2(pos.z, pos.y); // Left (-X)
    case 5u:
        return vec2(16.0 - pos.z, pos.y); // Right (+X)
    default:
        return pos.xy;
    }
}

void main() {
    Chunk chunk = chunks[gl_BaseInstance];

    uint localVertexID = uint(gl_VertexID) - chunk.baseVertex;
    uint localFaceIdx = localVertexID / 6u;
    uint globalFaceIdx = chunk.faceOffset + localFaceIdx;

    Face face = faces[globalFaceIdx];
    Vert vert = verts[gl_VertexID];

    vFace = float(face.face);
    vTex = float(face.texId);
    vBreakStage = float(face.breakStage);
    vLight = vert.light;

    vTexCoord = GetChunkUV(vert.pos, face.face, chunk.isUniform, chunk.baseVertex);

    gl_Position = uVP * vec4(vert.pos + (chunk.pos * 16.0), 1.0);
}