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
        this->blocks = std::make_unique<std::array<Block, Chunk::Volume>>();
        this->blocks->fill(BlockFromID(this->uniformType));
    }

    this->isUniform = false;
    this->isDirty = true;
}

Block ChunkData::GetBlock(BlockIndex idx) const {
    if (this->isUniform)
        return BlockFromID(this->uniformType);

    if (!this->blocks || this->blocks->empty()) {
        spdlog::warn("Attempted to get block of uninitialized chunk");
        return BlockFromID(0);
    }

    assert(idx.IsValid() && "Invalid block index");
    return this->blocks->at(idx.get());
};

Block ChunkData::GetBlock(const BlockPosition &pos) const {
    assert(pos.IsValid() && "Invalid block position");
    auto idx = pos.Idx();
    return GetBlock(idx);
};

void ChunkData::SetBlock(const BlockPosition &pos, BlockID blockId) {
    EnsureMutable();
    assert(pos.IsValid() && "Invalid block position");
    auto idx = pos.Idx();
    this->blocks->at(idx.get()) = BlockFromID(blockId);
    this->isDirty = true;
}

void ChunkData::SetBlockBreakStage(const BlockPosition &pos, uint8_t stage) {
    EnsureMutable();
    if (!pos.IsValid())
        return;

    assert(stage < 15 && "Invalid break stage");
    if (stage >= 14) {
        this->blocks->at(pos.Idx().get()) = BlockFromID(0);
    } else {
        this->blocks->at(pos.Idx().get()).breakStage = stage;
    }
}

uint8_t ChunkData::GetBlockBreakStage(const BlockPosition &pos) {
    if (!pos.IsValid())
        return 0;
    if (this->isUniform)
        return 0;
    return this->blocks->at(pos.Idx().get()).breakStage;
}