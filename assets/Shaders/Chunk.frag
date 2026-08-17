#version 450 core

in float vFace;
out vec4 FragColor;

vec3 FaceColor(int face) {
  if (face == 0)
    return vec3(0.85, 0.3, 0.3); // Front
  if (face == 1)
    return vec3(0.3, 0.75, 0.3); // Back
  if (face == 2)
    return vec3(0.3, 0.4, 0.9); // Top
  if (face == 3)
    return vec3(0.85, 0.8, 0.3); // Bottom
  if (face == 4)
    return vec3(0.8, 0.3, 0.8); // Left
  return vec3(0.3, 0.8, 0.8);   // Right
}

void main() {
  int face = int(vFace + 0.5);
  FragColor = vec4(FaceColor(face), 1.0);
}