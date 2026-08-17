#version 450 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in
    float aFace; // unsigned byte, unnormalized -> arrives as float (0..5)

layout(location = 0) uniform mat4 uMVP;
layout(location = 1) uniform vec3 uChunkPosition;

out float vFace;

void main() {
  vec3 worldPos = aPosition + uChunkPosition;
  gl_Position = uMVP * vec4(worldPos, 1.0);
  vFace = aFace;
}