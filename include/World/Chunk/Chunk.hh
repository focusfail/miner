#pragma once

#include "World/Coordinates.hh"

#include <array>
#include <cstdint>
#include <memory>

using ChunkBlockArray = std::unique_ptr<std::array<uint8_t, CHUNK_VOLUME>>;

struct ChunkData {
  bool isDirty = false;
  bool isUniform = true;
  uint8_t uniformType;
  ChunkPosition position;
  ChunkBlockArray blocks;

  ChunkData(const ChunkPosition &pos, uint8_t uniformType);
  void Fill(uint8_t type);
};