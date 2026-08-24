#include "World/Chunk/Chunk.hh"
#include "World/Block/Block.hh"
#include "World/Block/BlockRegistry.hh"
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
        this->blocks = std::make_unique<std::array<Block, CHUNK_VOLUME>>();
        this->blocks->fill(BlockFromID(this->uniformType));
    }

    this->isUniform = false;
}

Block Chunk::GetBlockRaw(BlockIndex idx) const {
    if (this->isUniform) return BlockFromID(this->uniformType);

    if (!this->blocks || this->blocks->empty()) {
        spdlog::warn("Attempted to get block of uninitialized chunk");
        return BlockFromID(0);
    }

    assert(idx.IsValid() && "Invalid block index");
    return this->blocks->at(idx.get());
};

Block Chunk::GetBlockRaw(const BlockPosition &pos) const {
    assert(pos.IsValid() && "Invalid block position");
    auto idx = pos.Idx();
    return GetBlockRaw(idx);
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

BlockInfo Chunk::GetBlock(BlockIndex idx) const {
    const auto &br = BlockRegistry::GetInstance();
    const auto &raw = GetBlockRaw(idx);
    const auto &entry = br.GetEntry(raw.id);

    return {.id = raw.id,
        .texId = entry.texId,
        .solid = entry.solid,
        .transparent = entry.transparent,
        .breakStage = raw.breakStage,
        .lightEmit = raw.lightEmit,
        .lightLv = raw.lightLv,
        .name = std::string_view(entry.name)};
}
BlockInfo Chunk::GetBlock(const BlockPosition &pos) const { return GetBlock(pos.Idx()); }

std::optional<BlockInfo> Chunk::TryGetBlockNeighbor(const BlockPosition &pos, const glm::ivec3 &offset) const {
    auto nbPos = BlockPosition(glm::ivec3(pos) + offset);
    if (!nbPos.IsValid()) return std::nullopt;

    return GetBlock(nbPos);
}

std::array<std::optional<BlockInfo>, 6> Chunk::GetBlockNeighbors(
    const BlockPosition &pos, const glm::ivec3 &offset) const {
    return {
        TryGetBlockNeighbor(pos, glm::ivec3{1, 0, 0}),  //
        TryGetBlockNeighbor(pos, glm::ivec3{-1, 0, 0}), //
        TryGetBlockNeighbor(pos, glm::ivec3{0, 1, 0}),  //
        TryGetBlockNeighbor(pos, glm::ivec3{0, -1, 0}), //
        TryGetBlockNeighbor(pos, glm::ivec3{0, 0, 1}),  //
        TryGetBlockNeighbor(pos, glm::ivec3{0, 0, -1}), //
    };
}

void Chunk::Randomize() {
    EnsureMutable();
    auto &br = BlockRegistry::GetInstance();
    int maxBlocks = br.GetBlockCount();

    for (size_t i = 0; i < CHUNK_VOLUME; i++) {
        BlockID randBlock = static_cast<BlockID>(rand() % maxBlocks);
        blocks->at(i).id = randBlock;
    }
}