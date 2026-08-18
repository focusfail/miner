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

    auto GetBlock(const BlockPosition &pos) const -> Block;
    auto GetBlock(int idx) const -> Block;

    void SetBlock(const BlockPosition &pos, BlockID block);

    void FillUniform(BlockID type);
    void EnsureMutable();
};