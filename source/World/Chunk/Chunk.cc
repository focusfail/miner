#include "World/Chunk/Chunk.hh"
#include "World/Coordinates.hh"
#include "spdlog/spdlog.h"
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

void ChunkData::ForceMutable() {

  if (!this->blocks || this->blocks->empty()) {
    spdlog::warn("[ChunkData:ForceMutable] Chunk doesn't have any blocks");
    this->blocks = std::make_unique<std::array<uint8_t, CHUNK_VOLUME>>();
    this->blocks->fill(this->uniformType);
  }

  this->isUniform = false;
  this->isDirty = true;
}