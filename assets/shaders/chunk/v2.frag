#version 460

flat in float vFace;
flat in float vTex;
flat in float vLight;
in vec2 vTexCoord;

out vec4 fragColor;

uniform sampler2DArray blockTexture;

void main() {
    vec4 color = texture(blockTexture, vec3(vTexCoord, vTex));
    fragColor = color * vec4(vec3(vLight), 1.0);
}