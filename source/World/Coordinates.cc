#include "World/Coordinates.hh"

auto BlockIdx2Pos(int i) -> BlockPosition {
  uint32_t x = i % CHUNK_SIZE;
  uint32_t y = (i / CHUNK_SIZE) % CHUNK_SIZE;
  uint32_t z = i / CHUNK_AREA;
  return {x, y, z};
}

auto BlockPos2Idx(const BlockPosition &p) -> int {
  return p.x + p.y * CHUNK_SIZE + p.z * CHUNK_AREA;
}

auto WorldPos2ChunkPos(glm::vec3 p) -> ChunkPosition {
  return {static_cast<int32_t>(std::floor(p.x / CHUNK_SIZE)),
          static_cast<int32_t>(std::floor(p.y / CHUNK_SIZE)),
          static_cast<int32_t>(std::floor(p.z / CHUNK_SIZE))};
}

auto WorldPos2ChunkAndBlock(glm::vec3 p)
    -> std::pair<ChunkPosition, BlockPosition> {
  ChunkPosition chunkPos = WorldPos2ChunkPos(p);

  glm::vec3 chunkOrigin = glm::vec3(chunkPos) * static_cast<float>(CHUNK_SIZE);

  glm::vec3 local = p - chunkOrigin;

  return {chunkPos, glm::uvec3(glm::floor(local))};
}