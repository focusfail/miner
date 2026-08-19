#pragma once

#include "World/Block/Block.hh"
#include "World/Coordinates.hh"

#include <array>
#include <cstdint>
#include <memory>

using ChunkBlockArray = std::unique_ptr<std::array<Block, CHUNK_VOLUME>>;

struct ChunkData {
    bool isDirty = false;
    bool isUniform = true;
    BlockID uniformType{};
    ChunkPosition position{};
    ChunkBlockArray blocks{};

    ChunkData(const ChunkPosition &pos, BlockID uniformType);

    Block GetBlock(const BlockPosition &pos) const;
    Block GetBlock(int idx) const;

    void SetBlock(const BlockPosition &pos, BlockID block);

    void FillUniform(BlockID type);
    void EnsureMutable();
};