#version 450 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in float aFace;

layout(location = 0) uniform mat4 uMVP;
layout(location = 1) uniform vec3 uChunkPosition;

out float vFace;
out vec2 vUV;

const vec2 QUAD_UVS[6] = vec2[6](
    vec2(0.0, 0.0), // 0: Bottom-Left
    vec2(1.0, 0.0), // 1: Bottom-Right
    vec2(1.0, 1.0), // 2: Top-Right

    vec2(1.0, 1.0), // 3: Top-Right
    vec2(0.0, 1.0), // 4: Top-Left
    vec2(0.0, 0.0)  // 5: Bottom-Left
);

void main() {
  vec3 worldPos = aPosition + uChunkPosition;
  gl_Position = uMVP * vec4(worldPos, 1.0);
  vFace = aFace;
  vUV = QUAD_UVS[gl_VertexID % 6];
}