#version 460

flat in float vFace;
flat in float vTex;
flat in float vLight;
flat in float vBreakStage;
in vec2 vTexCoord;

out vec4 fragColor;

layout(binding = 0) uniform sampler2DArray uBlockTexture;
layout(binding = 1) uniform sampler2DArray uOverTexture;

void main() {
    vec4 blockColor = texture(uBlockTexture, vec3(vTexCoord, vTex));

    if (vBreakStage > 0.0) {
        float overlayLayer = floor(((vBreakStage - 1.0) / 14.0) * 9.0);
        vec4 overlay = texture(uOverTexture, vec3(vTexCoord, overlayLayer));

        vec3 rgb = mix(blockColor.rgb, overlay.rgb, overlay.a);
        float alpha = max(blockColor.a, overlay.a);
        blockColor = vec4(rgb, alpha);
    }

    fragColor = blockColor * vec4(vec3(vLight), 1.0);
}