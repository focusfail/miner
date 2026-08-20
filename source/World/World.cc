#include "World/World.hh"
#include "World/Block/Block.hh"
#include "World/Chunk/Chunk.hh"
#include "World/Coordinates.hh"
#include "glm/geometric.hpp"
#include "spdlog/spdlog.h"

void World::Init() {
    m_ChunkRenderer.Init();

    m_BlockTextures.Init();

    m_ChunkManager.CreateChunk(ChunkPosition{0, 0, 0}, 0);
    m_ChunkManager.CreateChunk(ChunkPosition{0, 0, 1}, 1);
    m_ChunkManager.CreateChunk(ChunkPosition{0, 0, -1}, 1);
    m_ChunkManager.CreateChunk(ChunkPosition{1, 0, 0}, 1);
    m_ChunkManager.CreateChunk(ChunkPosition{-1, 0, 0}, 1);
    m_ChunkManager.CreateChunk(ChunkPosition{0, 1, 0}, 1);

    for (int i = 0; i < 5; i++) {
        m_ChunkManager.CreateChunk(ChunkPosition{0, -i, 0}, 1);
    }

    {
        auto &t = m_BlockTextures;
        auto &b = m_BlockRegistry;
        b.Register("air", false, true, -1);
        b.Register("stone", true, false, t.AddTexture("block:stone"));
    }
}

void World::Update() {
    auto dirty = m_ChunkManager.GetDirtyChunks();
    for (const auto &pos : dirty) {
        ChunkData &chunk = m_ChunkManager.GetChunkDataByPosition(pos);
        ChunkMeshData meshData =
            m_ChunkMesher.GenerateMesh(chunk, m_BlockRegistry);
        m_ChunkRenderer.UploadMesh(meshData);
        chunk.isDirty = false;
    }

    if (!dirty.empty()) {
        spdlog::info("[ChunkMesher] Meshed {} dirty chunks", dirty.size());
    }
}

void World::Render(const Camera &cam) { m_ChunkRenderer.Render(cam); }

void World::Destroy() { m_ChunkRenderer.Destroy(); }

ChunkData *World::TryGetChunkDataByPosition(const ChunkPosition &pos) {
    if (!m_ChunkManager.HasChunk(pos))
        return nullptr;

    return &m_ChunkManager.GetChunkDataByPosition(pos);
}
ChunkData *World::TryGetChunkDataByWorldPosition(glm::vec3 pos) {
    auto chunkPos = WorldPos2ChunkPos(pos);
    if (!m_ChunkManager.HasChunk(chunkPos))
        return nullptr;

    return &m_ChunkManager.GetChunkDataByPosition(chunkPos);
}

std::optional<HitResult> World::CastRay(glm::vec3 start, glm::vec3 end) {
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

    float tMaxX = (dir.x > 0.0f)
                      ? (std::floor(start.x) + 1.0f - start.x) * tDeltaX
                  : (dir.x < 0.0f) ? (start.x - std::floor(start.x)) * tDeltaX
                                   : 1e30f;

    float tMaxY = (dir.y > 0.0f)
                      ? (std::floor(start.y) + 1.0f - start.y) * tDeltaY
                  : (dir.y < 0.0f) ? (start.y - std::floor(start.y)) * tDeltaY
                                   : 1e30f;

    float tMaxZ = (dir.z > 0.0f)
                      ? (std::floor(start.z) + 1.0f - start.z) * tDeltaZ
                  : (dir.z < 0.0f) ? (start.z - std::floor(start.z)) * tDeltaZ
                                   : 1e30f;

    float currentDist = 0.0f;
    glm::ivec3 hitNormal(0);

    while (currentDist <= maxDist) {
        auto [chunkPos, blockPos] = WorldPos2ChunkAndBlock(x, y, z);

        Block block = BlockFromID(0);
        if (auto chunk = TryGetChunkDataByPosition(chunkPos)) {
            block = chunk->GetBlock(blockPos);
        }

        if (m_BlockRegistry.IsSolid(block.id)) {
            HitResult result;
            result.chunkPosition = chunkPos;
            result.blockPosition = blockPos;
            result.position = start + (dir * currentDist);
            result.normal = hitNormal;
            result.block = block;
            return result;
        }

        if (tMaxX < tMaxY) {
            if (tMaxX < tMaxZ) {
                currentDist = tMaxX;
                tMaxX += tDeltaX;
                x += stepX;
                hitNormal = glm::ivec3(-stepX, 0, 0);
            } else {
                currentDist = tMaxZ;
                tMaxZ += tDeltaZ;
                z += stepZ;
                hitNormal = glm::ivec3(0, 0, -stepZ);
            }
        } else {
            if (tMaxY < tMaxZ) {
                currentDist = tMaxY;
                tMaxY += tDeltaY;
                y += stepY;
                hitNormal = glm::ivec3(0, -stepY, 0);
            } else {
                currentDist = tMaxZ;
                tMaxZ += tDeltaZ;
                z += stepZ;
                hitNormal = glm::ivec3(0, 0, -stepZ);
            }
        }
    }

    return std::nullopt;
}

auto World::IsChunkLoaded(const ChunkPosition &pos) -> bool {
    return m_ChunkManager.HasChunk(pos);
}

auto World::IsBlockSolidAt(const glm::vec3 &worldPos) -> bool {
    auto [chunkPos, blockPos] = WorldPos2ChunkAndBlock(worldPos);
    ChunkData *chunk = TryGetChunkDataByPosition(chunkPos);
    if (!chunk)
        return false;

    Block block = chunk->GetBlock(blockPos);
    return m_BlockRegistry.IsSolid(block.id);
}