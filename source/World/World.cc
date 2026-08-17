#include "World/World.hh"

void World::Init() {
  m_ChunkRenderer.Init();
  // temporary: single test chunk at origin so there's something to render
  m_ChunkManager.CreateChunk(ChunkPosition{0, 0, 0}, 1);
  m_ChunkManager.CreateChunk(ChunkPosition{0, 0, 1}, 1);
  m_ChunkManager.CreateChunk(ChunkPosition{0, 1, 1}, 1);
  m_ChunkManager.CreateChunk(ChunkPosition{1, 1, 1}, 1);
  m_ChunkManager.CreateChunk(ChunkPosition{1, 0, 0}, 1);
  m_ChunkManager.CreateChunk(ChunkPosition{1, 1, 0}, 1);
}

void World::Update() {
  for (const auto &pos : m_ChunkManager.GetDirtyChunks()) {
    ChunkData &chunk = m_ChunkManager.GetChunkDataByPosition(pos);
    ChunkMeshData meshData = m_ChunkMesher.GenerateMesh(chunk);
    m_ChunkRenderer.UploadMesh(meshData);
    chunk.isDirty = false;
  }
}

void World::Render(const Camera &cam) { m_ChunkRenderer.Render(cam); }

void World::Destroy() { m_ChunkRenderer.Destroy(); }