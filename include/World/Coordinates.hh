#pragma once

#include <glm/glm.hpp>

#define CHUNK_SIZE 16 // # Blocks in 1d strip of a chunk
#define CHUNK_AREA                                                             \
  (CHUNK_SIZE * CHUNK_SIZE) // # Blocks in a 2d slice of a chunk
#define CHUNK_VOLUME (CHUNK_AREA * CHUNK_SIZE) // # Blocks in a whole chunk

using ChunkPosition = glm::ivec3;
using BlockPosition = glm::uvec3;

// Convert a block index in chunk to position in chunk
auto BlockIdx2Pos(int idx) -> BlockPosition;
auto BlockPos2Idx(const BlockPosition &pos) -> int;

auto WorldPos2ChunkPos(glm::vec3 position) -> ChunkPosition;
auto WorldPos2ChunkAndBlock(glm::vec3 p)
    -> std::pair<ChunkPosition, BlockPosition>;

enum class Face : uint8_t {
  Front = 0, // +x
  Back,      // -x
  Top,       // +y
  Bottom,    // -y
  Left,      // -z
  Right,     // +z
  Last,
};