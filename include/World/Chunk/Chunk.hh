#pragma once

#include "World/Block/Block.hh"
#include "World/Coordinates.hh"

#include <array>
#include <cstdint>
#include <memory>

using ChunkBlockArray = std::unique_ptr<std::array<Block, ChunkDim::Volume>>;

struct Chunk {
    bool isDirty = false;
    bool isUniform = true;
    BlockID uniformType{};
    ChunkPosition position{};
    ChunkBlockArray blocks{};

    Chunk(const ChunkPosition &pos, BlockID uniformType);

    Block GetBlock(const BlockPosition &pos) const;
    Block GetBlock(BlockIndex idx) const;

    void SetBlockBreakStage(const BlockPosition &pos, uint8_t stage);
    uint8_t GetBlockBreakStage(const BlockPosition &pos);
    void SetBlock(const BlockPosition &pos, BlockID block);
    void SetBlock(const BlockPosition &pos, Block block);
    void SetBlock(BlockIndex idx, Block block);

    void FillUniform(BlockID type);
    void EnsureMutable();
};