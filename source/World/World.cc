#include "World/World.hh"
#include "spdlog/spdlog.h"

void World::Init() {
  m_ChunkRenderer.Init();

  m_ChunkManager.CreateChunk(ChunkPosition{0, 0, 0}, 1);
  m_ChunkManager.CreateChunk(ChunkPosition{0, 0, 1}, 1);
  m_ChunkManager.CreateChunk(ChunkPosition{0, 1, 1}, 1);
  m_ChunkManager.CreateChunk(ChunkPosition{1, 1, 1}, 1);
  m_ChunkManager.CreateChunk(ChunkPosition{1, 0, 0}, 1);
  m_ChunkManager.CreateChunk(ChunkPosition{1, 1, 0}, 1);
}

void World::Update() {

  auto dirty = m_ChunkManager.GetDirtyChunks();
  for (const auto &pos : dirty) {
    ChunkData &chunk = m_ChunkManager.GetChunkDataByPosition(pos);
    ChunkMeshData meshData = m_ChunkMesher.GenerateMesh(chunk);
    m_ChunkRenderer.UploadMesh(meshData);
    chunk.isDirty = false;
  }

  if (!dirty.empty()) {
    spdlog::info("[ChunkMesher] Meshed {} dirty chunks", dirty.size());
  }
}

void World::Render(const Camera &cam) { m_ChunkRenderer.Render(cam); }

void World::Destroy() { m_ChunkRenderer.Destroy(); }

auto World::TryGetChunkDataByPosition(const ChunkPosition &pos)
    -> std::optional<ChunkData *> {
  if (!m_ChunkManager.HasChunk(pos))
    return {};

  return {&m_ChunkManager.GetChunkDataByPosition(pos)};
}