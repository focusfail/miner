#include "World/Coordinates.hh"

BlockPosition BlockIdx2Pos(int i) {
    uint32_t x = i % CHUNK_SIZE;
    uint32_t y = (i / CHUNK_SIZE) % CHUNK_SIZE;
    uint32_t z = i / CHUNK_AREA;
    return {x, y, z};
}

int BlockPos2Idx(const BlockPosition &p) {
    return p.x + p.y * CHUNK_SIZE + p.z * CHUNK_AREA;
}

ChunkPosition WorldPos2ChunkPos(glm::vec3 p) {
    int32_t x = static_cast<int32_t>(std::floor(p.x));
    int32_t y = static_cast<int32_t>(std::floor(p.y));
    int32_t z = static_cast<int32_t>(std::floor(p.z));

    auto floorDiv = [](int32_t coord, int32_t size) -> int32_t {
        return (coord < 0) ? ((coord - size + 1) / size) : (coord / size);
    };

    return {floorDiv(x, CHUNK_SIZE), floorDiv(y, CHUNK_SIZE),
            floorDiv(z, CHUNK_SIZE)};
}

std::pair<ChunkPosition, BlockPosition> WorldPos2ChunkAndBlock(glm::vec3 p) {

    int32_t x = static_cast<int32_t>(std::floor(p.x));
    int32_t y = static_cast<int32_t>(std::floor(p.y));
    int32_t z = static_cast<int32_t>(std::floor(p.z));

    // 2. Floor division for chunk coordinates
    auto floorDiv = [](int32_t coord, int32_t size) -> int32_t {
        return (coord < 0) ? ((coord - size + 1) / size) : (coord / size);
    };

    // 3. Euclidean modulo to wrap block positions strictly into [0, CHUNK_SIZE
    // - 1]
    auto euclidMod = [](int32_t coord, int32_t size) -> uint32_t {
        int32_t m = coord % size;
        return static_cast<uint32_t>(m < 0 ? m + size : m);
    };

    ChunkPosition chunkPos{floorDiv(x, CHUNK_SIZE), floorDiv(y, CHUNK_SIZE),
                           floorDiv(z, CHUNK_SIZE)};

    BlockPosition blockPos{euclidMod(x, CHUNK_SIZE), euclidMod(y, CHUNK_SIZE),
                           euclidMod(z, CHUNK_SIZE)};

    return {chunkPos, blockPos};
}

std::pair<ChunkPosition, BlockPosition>
WorldPos2ChunkAndBlock(int32_t x, int32_t y, int32_t z) {

    auto floorDiv = [](int32_t coord, int32_t size) -> int32_t {
        return (coord < 0) ? ((coord - size + 1) / size) : (coord / size);
    };

    auto euclidMod = [](int32_t coord, int32_t size) -> uint32_t {
        int32_t m = coord % size;
        return static_cast<uint32_t>(m < 0 ? m + size : m);
    };

    ChunkPosition chunkPos{floorDiv(x, CHUNK_SIZE), floorDiv(y, CHUNK_SIZE),
                           floorDiv(z, CHUNK_SIZE)};

    BlockPosition blockPos{euclidMod(x, CHUNK_SIZE), euclidMod(y, CHUNK_SIZE),
                           euclidMod(z, CHUNK_SIZE)};

    return {chunkPos, blockPos};
}