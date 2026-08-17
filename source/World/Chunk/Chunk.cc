#include "World/Chunk/Chunk.hh"
#include "World/Coordinates.hh"

ChunkData::ChunkData(const ChunkPosition &pos, uint8_t type) {
  this->isUniform = false;
  this->uniformType = type;
  this->position = pos;
  this->isDirty = true;
}

void ChunkData::Fill(uint8_t type) {
  if (this->blocks) {
    this->blocks.reset();
  }

  this->isUniform = true;
  this->uniformType = type;
  this->isDirty = true;
}