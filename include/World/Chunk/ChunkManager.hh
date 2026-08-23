#pragma once

#include "World/Chunk/Chunk.hh"
#include "World/Coordinates.hh"
#include <glm/gtx/hash.hpp>

#include <unordered_map>
#include <vector>

class ChunkManager {
public:
    Chunk &CreateChunk(const ChunkPosition &pos, uint8_t uniformType);
    std::vector<ChunkPosition> GetDirtyChunks();
    Chunk &GetChunkByPosition(const ChunkPosition &pos);
    bool HasChunk(const ChunkPosition &pos) const;

private:
    std::unordered_map<ChunkPosition, Chunk> m_Chunks;
};