#include "World/World.hh"
#include "World/Block/Block.hh"
#include "World/Chunk/Chunk.hh"
#include "World/Coordinates.hh"
#include "glm/geometric.hpp"
#include "spdlog/spdlog.h"

void World::Init() {
  m_ChunkRenderer.Init();

  m_ChunkManager.CreateChunk(ChunkPosition{0, 0, 0}, 0);
  m_ChunkManager.CreateChunk(ChunkPosition{1, 0, 1}, 1);
  m_ChunkManager.CreateChunk(ChunkPosition{0, 0, 1}, 1);
  m_ChunkManager.CreateChunk(ChunkPosition{0, 0, -1}, 1);
  m_ChunkManager.CreateChunk(ChunkPosition{1, 0, 0}, 1);
  m_ChunkManager.CreateChunk(ChunkPosition{-1, 0, 0}, 1);
  m_ChunkManager.CreateChunk(ChunkPosition{0, 1, 0}, 1);

  for (int i = -1; i > -10; i--) {
    m_ChunkManager.CreateChunk(ChunkPosition{0, i, 0}, 1);
  }

  m_BlockRegistry.Register("air", false, true);
  m_BlockRegistry.Register("stone", true, false);
}

void World::Update() {

  auto dirty = m_ChunkManager.GetDirtyChunks();
  for (const auto &pos : dirty) {
    ChunkData &chunk = m_ChunkManager.GetChunkDataByPosition(pos);
    ChunkMeshData meshData = m_ChunkMesher.GenerateMesh(chunk, m_BlockRegistry);
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

auto World::CastRay(glm::vec3 start, glm::vec3 end)
    -> std::optional<HitResult> {
  glm::vec3 dir = end - start;
  float maxDist = glm::length(dir);

  if (maxDist <= 0.0001f) {
    return std::nullopt;
  }

  dir /= maxDist;

  int x = static_cast<int>(std::floor(start.x));
  int y = static_cast<int>(std::floor(start.y));
  int z = static_cast<int>(std::floor(start.z));

  int stepX = (dir.x >= 0) ? 1 : -1;
  int stepY = (dir.y >= 0) ? 1 : -1;
  int stepZ = (dir.z >= 0) ? 1 : -1;

  float tDeltaX = (dir.x != 0.0f) ? std::abs(1.0f / dir.x) : 1e30f;
  float tDeltaY = (dir.y != 0.0f) ? std::abs(1.0f / dir.y) : 1e30f;
  float tDeltaZ = (dir.z != 0.0f) ? std::abs(1.0f / dir.z) : 1e30f;

  float tMaxX =
      (dir.x != 0.0f)
          ? std::abs((x + (stepX > 0 ? 1.0f : 0.0f) - start.x) / dir.x)
          : 1e30f;
  float tMaxY =
      (dir.y != 0.0f)
          ? std::abs((y + (stepY > 0 ? 1.0f : 0.0f) - start.y) / dir.y)
          : 1e30f;
  float tMaxZ =
      (dir.z != 0.0f)
          ? std::abs((z + (stepZ > 0 ? 1.0f : 0.0f) - start.z) / dir.z)
          : 1e30f;

  float currentDist = 0.0f;

  while (currentDist <= maxDist) {
    auto [chunkPos, blockPos] = WorldPos2ChunkAndBlock(glm::vec3(x, y, z));

    Block block = BlockFromID(0);
    auto chunkOpt = TryGetChunkDataByPosition(chunkPos);
    if (chunkOpt.has_value()) {
      block = chunkOpt.value()->GetBlock(blockPos);
    }

    if (m_BlockRegistry.IsSolid(block.id)) {
      HitResult result;
      result.chunkPosition = chunkPos;
      result.blockPosition = blockPos;
      result.position = start + (dir * currentDist);
      return result;
    }

    if (tMaxX < tMaxY) {
      if (tMaxX < tMaxZ) {
        currentDist = tMaxX;
        tMaxX += tDeltaX;
        x += stepX;
      } else {
        currentDist = tMaxZ;
        tMaxZ += tDeltaZ;
        z += stepZ;
      }
    } else {
      if (tMaxY < tMaxZ) {
        currentDist = tMaxY;
        tMaxY += tDeltaY;
        y += stepY;
      } else {
        currentDist = tMaxZ;
        tMaxZ += tDeltaZ;
        z += stepZ;
      }
    }
  }

  return std::nullopt;
}

auto World::IsChunkLoaded(const ChunkPosition &pos) -> bool {
  return m_ChunkManager.HasChunk(pos);
}