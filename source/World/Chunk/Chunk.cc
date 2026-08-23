#include "World/Chunk/Chunk.hh"
#include "World/Block/Block.hh"
#include "World/Coordinates.hh"
#include "spdlog/spdlog.h"
#include <cassert>
#include <memory>

Chunk::Chunk(const ChunkPosition &pos, BlockID type) {
    this->isUniform = true;
    this->uniformType = type;
    this->position = pos;
    this->isDirty = true;
}

void Chunk::FillUniform(BlockID type) {
    if (this->blocks) {
        this->blocks.reset();
    }

    this->isUniform = true;
    this->uniformType = type;
    this->isDirty = true;
}

void Chunk::EnsureMutable() {
    if (!this->blocks) {
        this->blocks = std::make_unique<std::array<Block, ChunkDim::Volume>>();
        this->blocks->fill(BlockFromID(this->uniformType));
    }

    this->isUniform = false;
}

Block Chunk::GetBlock(BlockIndex idx) const {
    if (this->isUniform) return BlockFromID(this->uniformType);

    if (!this->blocks || this->blocks->empty()) {
        spdlog::warn("Attempted to get block of uninitialized chunk");
        return BlockFromID(0);
    }

    assert(idx.IsValid() && "Invalid block index");
    return this->blocks->at(idx.get());
};

Block Chunk::GetBlock(const BlockPosition &pos) const {
    assert(pos.IsValid() && "Invalid block position");
    auto idx = pos.Idx();
    return GetBlock(idx);
};

void Chunk::SetBlock(const BlockPosition &pos, BlockID blockId) {
    EnsureMutable();
    assert(pos.IsValid() && "Invalid block position");
    auto idx = pos.Idx();
    this->blocks->at(idx.get()) = BlockFromID(blockId);
    this->isDirty = true;
}

void Chunk::SetBlock(const BlockPosition &pos, Block block) {
    EnsureMutable();
    assert(pos.IsValid() && "Invalid block position");
    auto idx = pos.Idx();
    this->blocks->at(idx.get()) = block;
    this->isDirty = true;
}

void Chunk::SetBlock(BlockIndex idx, Block block) {
    EnsureMutable();
    assert(idx.IsValid() && "Invalid block position");
    this->blocks->at(idx.get()) = block;
    this->isDirty = true;
}

void Chunk::SetBlockBreakStage(const BlockPosition &pos, uint8_t stage) {
    EnsureMutable();
    if (!pos.IsValid()) return;

    if (stage >= 14) {
        this->blocks->at(pos.Idx().get()) = BlockFromID(0);
        this->isDirty = true;
    } else {
        this->blocks->at(pos.Idx().get()).breakStage = stage;
    }
}

uint8_t Chunk::GetBlockBreakStage(const BlockPosition &pos) {
    if (!pos.IsValid()) return 0;
    if (this->isUniform) return 0;
    return this->blocks->at(pos.Idx().get()).breakStage;
}
