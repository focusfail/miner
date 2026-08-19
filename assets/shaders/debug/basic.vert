#version 450

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aCol;

layout(location = 0) uniform mat4 uVP;

out vec4 vColor;

void main() {
    gl_Position = uVP * vec4(aPos, 1.0);
    vColor = aCol;
}
