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

  auto GetBlock(const BlockPosition &pos) const -> uint32_t;
  auto GetBlock(int idx) const -> uint32_t;

  void SetBlock(const BlockPosition &pos, uint32_t block);

  void FillUniform(uint8_t type);
  void EnsureMutable();
};