#include "World/World.hh"
#include "World/Block/Block.hh"
#include "World/Chunk/Chunk.hh"
#include "World/Coordinates.hh"
#include "glm/geometric.hpp"
#include "spdlog/spdlog.h"
#include <queue>

struct LightNode {
    BlockIndex idx;
    Chunk *chunk;
};

void World::Init() {
    m_ChunkRenderer.Init();

    m_ChunkManager.CreateChunk(ChunkPosition{0, 0, 0}, 0);
    m_ChunkManager.CreateChunk(ChunkPosition{0, 0, 1}, 1);
    m_ChunkManager.CreateChunk(ChunkPosition{0, 0, -1}, 1);
    m_ChunkManager.CreateChunk(ChunkPosition{1, 0, 0}, 1);
    m_ChunkManager.CreateChunk(ChunkPosition{-1, 0, 0}, 1);
    m_ChunkManager.CreateChunk(ChunkPosition{0, 1, 0}, 2);

    for (int i = 0; i < 5; i++) {
        m_ChunkManager.CreateChunk(ChunkPosition{0, -i, 0}, 1);
    }

    m_BlockTextures.Init();

    auto &t = m_BlockTextures;
    auto &b = m_BlockRegistry;
    b.Register("air", false, true, -1);
    b.Register("stone", true, false, t.AddTexture("block:stone"));
    b.Register("stone_bricks", true, false, t.AddTexture("block:stone_bricks"));

    m_ChunkRenderer.SetTextureArray(&m_BlockTextures);
}

void World::Update() {
    auto dirty = m_ChunkManager.GetDirtyChunks();
    for (const auto &pos : dirty) {
        Chunk *nbs[6] = {
            TryGetChunk(pos + glm::ivec3{1, 0, 0}),
            TryGetChunk(pos + glm::ivec3{-1, 0, 0}),
            TryGetChunk(pos + glm::ivec3{0, 1, 0}),
            TryGetChunk(pos + glm::ivec3{0, -1, 0}),
            TryGetChunk(pos + glm::ivec3{0, 0, 1}),
            TryGetChunk(pos + glm::ivec3{0, 0, -1}),
        };

        Chunk &chunk = m_ChunkManager.GetChunkByPosition(pos);
        PropagateLight(pos);
        ChunkMeshData meshData = m_ChunkMesher.GenerateMesh(chunk, m_BlockRegistry, nbs);
        m_ChunkRenderer.UploadMesh(meshData);
        chunk.isDirty = false;
    }

    if (!dirty.empty()) {
        spdlog::info("[ChunkMesher] Meshed {} dirty chunks", dirty.size());
    }
}

void World::Render(const Camera &cam) { m_ChunkRenderer.Render(cam); }

void World::Destroy() { m_ChunkRenderer.Destroy(); }

Chunk *World::TryGetChunk(const ChunkPosition &pos) {
    if (!m_ChunkManager.HasChunk(pos)) return nullptr;

    return &m_ChunkManager.GetChunkByPosition(pos);
}
Chunk *World::TryGetChunk(glm::vec3 pos) {
    auto chunkPos = WorldPos2ChunkPos(pos);
    if (!m_ChunkManager.HasChunk(chunkPos)) return nullptr;

    return &m_ChunkManager.GetChunkByPosition(chunkPos);
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

    float tMaxX = (dir.x > 0.0f)   ? (std::floor(start.x) + 1.0f - start.x) * tDeltaX
                  : (dir.x < 0.0f) ? (start.x - std::floor(start.x)) * tDeltaX
                                   : 1e30f;

    float tMaxY = (dir.y > 0.0f)   ? (std::floor(start.y) + 1.0f - start.y) * tDeltaY
                  : (dir.y < 0.0f) ? (start.y - std::floor(start.y)) * tDeltaY
                                   : 1e30f;

    float tMaxZ = (dir.z > 0.0f)   ? (std::floor(start.z) + 1.0f - start.z) * tDeltaZ
                  : (dir.z < 0.0f) ? (start.z - std::floor(start.z)) * tDeltaZ
                                   : 1e30f;

    float currentDist = 0.0f;
    glm::ivec3 hitNormal(0);

    while (currentDist <= maxDist) {
        auto [chunkPos, blockPos] = WorldPos2ChunkAndBlock(x, y, z);

        Block block = BlockFromID(0);
        if (auto chunk = TryGetChunk(chunkPos)) {
            block = chunk->GetBlock(blockPos);
        }

        if (m_BlockRegistry.IsSolid(block.id)) {
            HitResult result;
            result.chunkPos = chunkPos;
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

auto World::IsChunkLoaded(const ChunkPosition &pos) -> bool { return m_ChunkManager.HasChunk(pos); }

auto World::IsBlockSolidAt(const glm::vec3 &worldPos) -> bool {
    auto [chunkPos, blockPos] = WorldPos2ChunkAndBlock(worldPos);
    Chunk *chunk = TryGetChunk(chunkPos);
    if (!chunk) return false;

    Block block = chunk->GetBlock(blockPos);
    return m_BlockRegistry.IsSolid(block.id);
}

void World::PropagateLight(const ChunkPosition &chunkPos) {
    auto chunk = TryGetChunk(chunkPos);
    if (!chunk && !chunk->isDirty) return;

    std::queue<LightNode> lightQueue;

    for (size_t i = 0; i < ChunkDim::Volume; i++) {
        auto blockIdx = BlockIndex(i);
        auto block = chunk->GetBlock(blockIdx);

        if (block.lightEmit == 0) continue;

        block.lightLv = block.lightEmit;
        chunk->SetBlock(blockIdx, block);
        lightQueue.push({blockIdx, chunk});
    }

    static const glm::vec3 offsets[6] = {{1, 0, 0}, {-1, 0, 0}, {0, 1, 0}, {0, -1, 0}, {0, 0, 1}, {0, 0, -1}};

    while (!lightQueue.empty()) {
        LightNode node = lightQueue.front();
        lightQueue.pop();
        chunk = node.chunk;

        auto block = chunk->GetBlock(node.idx);
        uint8_t currentLight = block.lightLv;

        if (currentLight <= 1) continue;

        auto pos = node.idx.Pos();
        if (!pos.has_value()) {
            spdlog::info("Pos() failed for idx!");
            continue;
        }

        for (const auto &off : offsets) {
            auto wp = off + glm::vec3(*pos) + (glm::vec3(chunkPos) * static_cast<float>(ChunkDim::Size));

            auto [nbPos, blockPos] = WorldPos2ChunkAndBlock(wp);
            if (auto nbChunk = TryGetChunk(nbPos)) {
                BlockIndex neighborIdx = blockPos.Idx();
                auto neighbor = nbChunk->GetBlock(neighborIdx);

                nbChunk->EnsureMutable();
                nbChunk->isDirty = true;

                if (neighbor.id != 0) {
                    continue;
                }

                uint8_t newLight = currentLight - 1;

                if (newLight > neighbor.lightLv) {
                    neighbor.lightLv = newLight;
                    nbChunk->SetBlock(neighborIdx, neighbor);
                    lightQueue.push({neighborIdx, nbChunk});
                }
            }
        }
    }
}