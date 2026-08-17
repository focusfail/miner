#pragma once

#include "World/Chunk/Chunk.hh"
#include "World/Coordinates.hh"
#include <glm/gtx/hash.hpp>

#include <unordered_map>
#include <vector>

class ChunkManager {
public:
  auto CreateChunk(const ChunkPosition &pos, uint8_t uniformType)
      -> ChunkData &;
  auto GetDirtyChunks() -> std::vector<ChunkPosition>;
  auto GetChunkDataByPosition(const ChunkPosition &pos) -> ChunkData &;
  auto HasChunk(const ChunkPosition &pos) const -> bool;

private:
  std::unordered_map<ChunkPosition, ChunkData> m_Chunks;
};