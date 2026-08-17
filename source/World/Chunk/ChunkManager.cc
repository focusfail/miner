#include "World/Chunk/ChunkManager.hh"

auto ChunkManager::CreateChunk(const ChunkPosition &pos, uint8_t uniformType)
    -> ChunkData & {
  auto [it, inserted] = m_Chunks.try_emplace(pos, pos, uniformType);
  return it->second;
}

auto ChunkManager::GetDirtyChunks() -> std::vector<ChunkPosition> {
  std::vector<ChunkPosition> dirty;
  for (auto &[pos, chunk] : m_Chunks) {
    if (chunk.isDirty) {
      dirty.push_back(pos);
    }
  }
  return dirty;
}

auto ChunkManager::GetChunkDataByPosition(const ChunkPosition &pos)
    -> ChunkData & {
  return m_Chunks.at(pos);
}

auto ChunkManager::HasChunk(const ChunkPosition &pos) const -> bool {
  return m_Chunks.find(pos) != m_Chunks.end();
}