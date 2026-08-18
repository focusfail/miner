#include "World/Chunk/Chunk.hh"
#include "World/Coordinates.hh"
#include "spdlog/spdlog.h"
#include <cassert>
#include <memory>

ChunkData::ChunkData(const ChunkPosition &pos, uint8_t type) {
  this->isUniform = true;
  this->uniformType = type;
  this->position = pos;
  this->isDirty = true;
}

void ChunkData::FillUniform(uint8_t type) {
  if (this->blocks) {
    this->blocks.reset();
  }

  this->isUniform = true;
  this->uniformType = type;
  this->isDirty = true;
}

void ChunkData::EnsureMutable() {

  if (!this->blocks || this->blocks->empty()) {
    this->blocks = std::make_unique<std::array<uint8_t, CHUNK_VOLUME>>();
    this->blocks->fill(this->uniformType);
  }

  this->isUniform = false;
  this->isDirty = true;
}

auto ChunkData::GetBlock(int idx) const -> uint32_t {
  if (this->isUniform)
    return this->uniformType;

  if (!this->blocks || this->blocks->empty()) {
    spdlog::warn("Attempted to get block of uninitialized chunk");
    return 0;
  }

  assert(idx >= 0 && idx < CHUNK_VOLUME && "Invalid block position");
  return this->blocks->at(idx);
};

auto ChunkData::GetBlock(const BlockPosition &pos) const -> uint32_t {
  int idx = BlockPos2Idx(pos);
  return GetBlock(idx);
};

void ChunkData::SetBlock(const BlockPosition &pos, uint32_t block) {
  EnsureMutable();
  int idx = BlockPos2Idx(pos);
  assert(idx >= 0 && idx < CHUNK_VOLUME && "Invalid block position");
  this->blocks->at(idx) = block;
  this->isDirty = true;
}