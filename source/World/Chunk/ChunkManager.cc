#include "World/Chunk/ChunkManager.hh"

#include <spdlog/spdlog.h>

Chunk &ChunkManager::CreateChunk(const ChunkPosition &pos, uint8_t uniformType) {
    spdlog::info("[ChunkManager] Created chunk at ({},{},{})", pos.x, pos.y, pos.z);
    auto [it, inserted] = m_Chunks.try_emplace(pos, pos, uniformType);
    return it->second;
}

std::vector<ChunkPosition> ChunkManager::GetDirtyChunks() {
    std::vector<ChunkPosition> dirty;
    for (auto &[pos, chunk] : m_Chunks) {
        if (chunk.isDirty) {
            dirty.push_back(pos);
        }
    }
    return dirty;
}

Chunk &ChunkManager::GetChunkByPosition(const ChunkPosition &pos) { return m_Chunks.at(pos); }

bool ChunkManager::HasChunk(const ChunkPosition &pos) const { return m_Chunks.find(pos) != m_Chunks.end(); }