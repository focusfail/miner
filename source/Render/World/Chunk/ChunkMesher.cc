#include "Render/World/Chunk/ChunkMesher.hh"
#include "Render/World/Chunk/ChunkMesh.hh"
#include "World/Block/BlockRegistry.hh"
#include "World/Coordinates.hh"
#include "spdlog/spdlog.h"
#include <cstdint>

static const std::array<ChunkMeshVertex, 36> blockVertices = {{
    // Front (+Z)
    {{0, 0, 1}}, // Bottom-Left
    {{1, 0, 1}}, // Bottom-Right
    {{1, 1, 1}}, // Top-Right
    {{1, 1, 1}}, // Top-Right
    {{0, 1, 1}}, // Top-Left
    {{0, 0, 1}}, // Bottom-Left

    // Back (-Z)
    {{1, 0, 0}}, // Bottom-Left
    {{0, 0, 0}}, // Bottom-Right
    {{0, 1, 0}}, // Top-Right
    {{0, 1, 0}}, // Top-Right
    {{1, 1, 0}}, // Top-Left
    {{1, 0, 0}}, // Bottom-Left

    // Top (+Y)
    {{0, 1, 1}}, // Bottom-Left  (Front-Left)
    {{1, 1, 1}}, // Bottom-Right (Front-Right)
    {{1, 1, 0}}, // Top-Right    (Back-Right)
    {{1, 1, 0}}, // Top-Right    (Back-Right)
    {{0, 1, 0}}, // Top-Left     (Back-Left)
    {{0, 1, 1}}, // Bottom-Left  (Front-Left)

    // Bottom (-Y)
    {{0, 0, 0}}, // Bottom-Left  (Back-Left)
    {{1, 0, 0}}, // Bottom-Right (Back-Right)
    {{1, 0, 1}}, // Top-Right    (Front-Right)
    {{1, 0, 1}}, // Top-Right    (Front-Right)
    {{0, 0, 1}}, // Top-Left     (Front-Left)
    {{0, 0, 0}}, // Bottom-Left  (Back-Left)

    // Left (-X)
    {{0, 0, 0}}, // Bottom-Left  (Back-Bottom)
    {{0, 0, 1}}, // Bottom-Right (Front-Bottom)
    {{0, 1, 1}}, // Top-Right    (Front-Top)
    {{0, 1, 1}}, // Top-Right    (Front-Top)
    {{0, 1, 0}}, // Top-Left     (Back-Top)
    {{0, 0, 0}}, // Bottom-Left  (Back-Bottom)

    // Right (+X)
    {{1, 0, 1}}, // Bottom-Left  (Front-Bottom)
    {{1, 0, 0}}, // Bottom-Right (Back-Bottom)
    {{1, 1, 0}}, // Top-Right    (Back-Top)
    {{1, 1, 0}}, // Top-Right    (Back-Top)
    {{1, 1, 1}}, // Top-Left     (Front-Top)
    {{1, 0, 1}}, // Bottom-Left  (Front-Bottom)
}};

std::array<bool, 6> GetShouldCull(const Chunk &chunk, BlockRegistry &reg, const BlockPosition &pos) {
    constexpr glm::ivec3 offs[6] = {
        {0, 0, 1},
        {0, 0, -1},
        {0, 1, 0},
        {0, -1, 0},
        {-1, 0, 0},
        {1, 0, 0},
    };

    std::array<bool, 6> shouldCull{};

    for (int i = 0; i < 6; ++i) {
        const glm::ivec3 neighbor = glm::ivec3(pos) + offs[i];

        if (neighbor.x < 0 || neighbor.x >= CHUNK_SIZE || neighbor.y < 0 || neighbor.y >= CHUNK_SIZE ||
            neighbor.z < 0 || neighbor.z >= CHUNK_SIZE) {

            shouldCull[i] = false;
            continue;
        }

        BlockPosition neighborPos(
            static_cast<uint8_t>(neighbor.x), static_cast<uint8_t>(neighbor.y), static_cast<uint8_t>(neighbor.z));

        BlockIndex idx = neighborPos.Idx();

        auto block = chunk.GetBlockRaw(idx);
        shouldCull[i] = !reg.IsTransparent(block.id);
    }

    return shouldCull;
}

ChunkMeshData ChunkMesher::GenerateMesh(const Chunk &chunk, std::array<Chunk *, 6> &nbChunks) {
    auto &br = BlockRegistry::GetInstance();
    static const glm::vec3 offsets[6] = {{1, 0, 0}, {-1, 0, 0}, {0, 1, 0}, {0, -1, 0}, {0, 0, 1}, {0, 0, -1}};

    ChunkMeshData mesh;
    mesh.position = chunk.position;
    mesh.chunk.isUniform = chunk.isUniform;
    mesh.chunk.position = chunk.position;

    if (chunk.isUniform && !br.IsTransparent(chunk.uniformType)) {

        mesh.vertices.reserve(blockVertices.size());

        for (int face = 0; face != static_cast<int>(Face::Last); face++) {
            for (int faceVertexIndex = 0; faceVertexIndex < 6; faceVertexIndex++) {
                int vertexIndex = faceVertexIndex + face * 6;
                const auto &ogVertex = blockVertices[vertexIndex];
                glm::vec3 newPos = ogVertex.position * static_cast<float>(CHUNK_SIZE);
                mesh.vertices.emplace_back(ChunkMeshVertex{newPos});
            }
            mesh.faces.emplace_back(static_cast<uint32_t>(face), br.GetTexture(chunk.uniformType), 0);
        }

        return mesh;
    }

    mesh.vertices.reserve(blockVertices.size() * 2048);

    for (size_t i = 0; i < chunk.blocks->size(); i++) {
        BlockIndex blockIndex = BlockIndex(i);
        if (!blockIndex.IsValid()) continue;

        auto block = chunk.GetBlockRaw(blockIndex);
        auto maybePos = blockIndex.Pos();

        if (br.IsTransparent(block.id) || !maybePos.has_value()) continue;

        auto pos = *maybePos;

        auto shouldCull = GetShouldCull(chunk, br, pos);
        for (int face = 0; face != static_cast<int>(Face::Last); face++) {
            if (shouldCull[face]) continue;

            auto nbPos = offsets[face] + glm::vec3(pos) + glm::vec3(chunk.position) * static_cast<float>(CHUNK_SIZE);
            auto [nbChunkPos, nbBlockPos] = WorldPos2ChunkAndBlock(nbPos);

            Chunk *nbChunk = (nbChunkPos == chunk.position) ? const_cast<Chunk *>(&chunk) : nbChunks[face];

            Block dflt{};
            int lightLv = dflt.lightLv;
            if (nbChunk) {
                auto nbBlock = nbChunk->GetBlockRaw(nbBlockPos);
                lightLv = nbBlock.lightLv;
            }

            for (int faceVertexIndex = 0; faceVertexIndex < 6; faceVertexIndex++) {
                int vertexIndex = faceVertexIndex + face * 6;
                const auto &ogVertex = blockVertices[vertexIndex];
                glm::vec3 newPos = ogVertex.position + glm::vec3(pos);
                mesh.vertices.emplace_back(
                    ChunkMeshVertex{.position = newPos, .light = static_cast<float>(lightLv) / 15.0f});
            }
            mesh.faces.emplace_back(
                static_cast<uint32_t>(face), br.GetTexture(block.id), static_cast<uint32_t>(block.breakStage));
        }
    }

    return mesh;
}