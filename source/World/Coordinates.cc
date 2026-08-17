#include "World/Coordinates.hh"

auto BlockIdx2Pos(int i) -> BlockPosition {
  uint32_t x = i % CHUNK_SIZE;
  uint32_t y = (i / CHUNK_SIZE) % CHUNK_SIZE;
  uint32_t z = i / CHUNK_AREA;
  return {x, y, z};
}