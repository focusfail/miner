#pragma once

#include "Render/World/Chunk/ChunkMesh.hh"
#include "World/Chunk/Chunk.hh"

class ChunkMesher {
public:
  ChunkMesher() = default;
  ~ChunkMesher() = default;

  auto GenerateMesh(const ChunkData &chunk) -> ChunkMeshData;
};