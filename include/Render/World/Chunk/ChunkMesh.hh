#pragma once

#include "World/Coordinates.hh"

#include <cstdint>
#include <glm/ext/vector_float3.hpp>

struct ChunkMeshVertex {
  glm::vec3 position;
  Face::Face face;
};

struct ChunkMeshData {
  ChunkPosition position;
  std::vector<ChunkMeshVertex> vertices;
};

struct ChunkMesh {
  uint32_t numVertices = 0;
  uint32_t vbo = 0;
  uint32_t vao = 0;
  ChunkPosition position;

  void Upload(const ChunkMeshData &data);
  void Render();

  void Destroy();
};