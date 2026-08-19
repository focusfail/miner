#pragma once

#include "spdlog/spdlog.h"
#include <cstddef>
#include <glm/glm.hpp>
#include <optional>

constexpr int32_t FloorDiv(int32_t value, int32_t divisor) {
    return value >= 0 ? value / divisor : (value - divisor + 1) / divisor;
}

constexpr uint32_t EuclideanMod(int32_t value, int32_t divisor) {
    const int32_t remainder = value % divisor;
    return static_cast<uint32_t>(remainder < 0 ? remainder + divisor
                                               : remainder);
}

namespace Chunk {
inline constexpr std::size_t Size = 16;
inline constexpr std::size_t Area = Size * Size;
inline constexpr std::size_t Volume = Area * Size;
}; // namespace Chunk

using ChunkPosition = glm::ivec3;

struct BlockIndex;

struct BlockPosition {
    glm::uvec3 v = {0, 0, 0};

    constexpr BlockPosition() = default;
    constexpr explicit BlockPosition(const glm::uvec3 &v) : v(v) {}
    constexpr explicit BlockPosition(uint8_t x, uint8_t y, uint8_t z)
        : v(x, y, z) {}

    constexpr explicit operator glm::vec3() const noexcept {
        return glm::vec3(v);
    }
    constexpr explicit operator glm::ivec3() const noexcept {
        return glm::ivec3(v);
    }

    constexpr bool IsValid() const {
        return v.x < Chunk::Size && v.y < Chunk::Size && v.z < Chunk::Size;
    }
    constexpr BlockIndex Idx() const;
};

struct BlockIndex {
    uint16_t v = 0;

    constexpr BlockIndex() = default;
    constexpr explicit BlockIndex(uint16_t v) : v(v) {}

    constexpr bool IsValid() const { return v < Chunk::Volume; }
    constexpr std::optional<BlockPosition> Pos() const {
        if (!IsValid())
            return std::nullopt;

        uint32_t x = v % Chunk::Size;
        uint32_t y = (v / Chunk::Size) % Chunk::Size;
        uint32_t z = v / Chunk::Area;

        BlockPosition pos = BlockPosition(x, y, z);

        if (!pos.IsValid())
            return std::nullopt;

        return pos;
    }

    constexpr explicit operator std::size_t() const noexcept {
        return static_cast<std::size_t>(v);
    }

    [[nodiscard]] constexpr std::size_t get() const noexcept {
        return static_cast<std::size_t>(v);
    }

    constexpr bool operator==(const BlockIndex &other) const = default;
    constexpr bool operator!=(const BlockIndex &other) const = default;
    constexpr bool operator<(const BlockIndex &other) const {
        return v < other.v;
    };
};

constexpr BlockIndex BlockPosition::Idx() const {
    return BlockIndex(v.x + v.y * Chunk::Size + v.z * Chunk::Area);
}

constexpr ChunkPosition WorldPos2ChunkPos(const glm::vec3 &p) {
    const int32_t x = static_cast<int32_t>(std::floor(p.x));
    const int32_t y = static_cast<int32_t>(std::floor(p.y));
    const int32_t z = static_cast<int32_t>(std::floor(p.z));

    return {FloorDiv(x, Chunk::Size), FloorDiv(y, Chunk::Size),
            FloorDiv(z, Chunk::Size)};
}

constexpr std::pair<ChunkPosition, BlockPosition>
WorldPos2ChunkAndBlock(int32_t x, int32_t y, int32_t z) {
    ChunkPosition chunkPos{FloorDiv(x, Chunk::Size), FloorDiv(y, Chunk::Size),
                           FloorDiv(z, Chunk::Size)};

    BlockPosition blockPos{static_cast<uint8_t>(EuclideanMod(x, Chunk::Size)),
                           static_cast<uint8_t>(EuclideanMod(y, Chunk::Size)),
                           static_cast<uint8_t>(EuclideanMod(z, Chunk::Size))};

    return {chunkPos, blockPos};
}

constexpr std::pair<ChunkPosition, BlockPosition>
WorldPos2ChunkAndBlock(const glm::vec3 &p) {
    return WorldPos2ChunkAndBlock(static_cast<int32_t>(std::floor(p.x)),
                                  static_cast<int32_t>(std::floor(p.y)),
                                  static_cast<int32_t>(std::floor(p.z)));
}

enum class Face : uint8_t {
    Front = 0, // +z
    Back,      // -z
    Top,       // +y
    Bottom,    // -y
    Left,      // -x
    Right,     // +x
    Last,
};
