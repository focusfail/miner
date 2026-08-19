#include "World/Chunk/Chunk.hh"
#include "World/Block/Block.hh"
#include "World/Coordinates.hh"
#include "spdlog/spdlog.h"
#include <cassert>
#include <memory>

ChunkData::ChunkData(const ChunkPosition &pos, BlockID type) {
    this->isUniform = true;
    this->uniformType = type;
    this->position = pos;
    this->isDirty = true;
}

void ChunkData::FillUniform(BlockID type) {
    if (this->blocks) {
        this->blocks.reset();
    }

    this->isUniform = true;
    this->uniformType = type;
    this->isDirty = true;
}

void ChunkData::EnsureMutable() {
    if (!this->blocks) {
        this->blocks = std::make_unique<std::array<Block, CHUNK_VOLUME>>();
        this->blocks->fill(BlockFromID(this->uniformType));
    }

    this->isUniform = false;
    this->isDirty = true;
}

Block ChunkData::GetBlock(int idx) const {
    if (this->isUniform)
        return BlockFromID(this->uniformType);

    if (!this->blocks || this->blocks->empty()) {
        spdlog::warn("Attempted to get block of uninitialized chunk");
        return BlockFromID(0);
    }

    assert(idx >= 0 && idx < CHUNK_VOLUME && "Invalid block position");
    return this->blocks->at(idx);
};

Block ChunkData::GetBlock(const BlockPosition &pos) const {
    int idx = BlockPos2Idx(pos);
    return GetBlock(idx);
};

void ChunkData::SetBlock(const BlockPosition &pos, BlockID blockId) {
    EnsureMutable();
    int idx = BlockPos2Idx(pos);
    assert(idx >= 0 && idx < CHUNK_VOLUME && "Invalid block position");
    this->blocks->at(idx) = BlockFromID(blockId);
    this->isDirty = true;
}