#pragma once

#include "glm/ext/vector_float3.hpp"

struct Capsule {
  glm::vec3 position;
  float height;
  float radius;

  Capsule() = default;
  Capsule(glm::vec3 pos, float h, float r)
      : position(pos), height(h), radius(r) {}

  void GetAABB(glm::vec3 &outMin, glm::vec3 &outMax) const {
    outMin = position - glm::vec3(radius, 0.0f, radius);
    outMax = position + glm::vec3(radius, height, radius);
  }
};