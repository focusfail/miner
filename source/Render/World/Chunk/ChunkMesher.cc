#include "Render/World/Chunk/ChunkMesher.hh"
#include "Render/World/Chunk/ChunkMesh.hh"
#include "World/Coordinates.hh"
#include "spdlog/spdlog.h"

static const std::array<ChunkMeshVertex, 36> blockVertices = {{
    // Front (+Z)
    {{0, 0, 1}, Face::Front}, // Bottom-Left
    {{1, 0, 1}, Face::Front}, // Bottom-Right
    {{1, 1, 1}, Face::Front}, // Top-Right
    {{1, 1, 1}, Face::Front}, // Top-Right
    {{0, 1, 1}, Face::Front}, // Top-Left
    {{0, 0, 1}, Face::Front}, // Bottom-Left

    // Back (-Z)
    {{1, 0, 0}, Face::Back}, // Bottom-Left
    {{0, 0, 0}, Face::Back}, // Bottom-Right
    {{0, 1, 0}, Face::Back}, // Top-Right
    {{0, 1, 0}, Face::Back}, // Top-Right
    {{1, 1, 0}, Face::Back}, // Top-Left
    {{1, 0, 0}, Face::Back}, // Bottom-Left

    // Top (+Y)
    {{0, 1, 1}, Face::Top}, // Bottom-Left  (Front-Left)
    {{1, 1, 1}, Face::Top}, // Bottom-Right (Front-Right)
    {{1, 1, 0}, Face::Top}, // Top-Right    (Back-Right)
    {{1, 1, 0}, Face::Top}, // Top-Right    (Back-Right)
    {{0, 1, 0}, Face::Top}, // Top-Left     (Back-Left)
    {{0, 1, 1}, Face::Top}, // Bottom-Left  (Front-Left)

    // Bottom (-Y)
    {{0, 0, 0}, Face::Bottom}, // Bottom-Left  (Back-Left)
    {{1, 0, 0}, Face::Bottom}, // Bottom-Right (Back-Right)
    {{1, 0, 1}, Face::Bottom}, // Top-Right    (Front-Right)
    {{1, 0, 1}, Face::Bottom}, // Top-Right    (Front-Right)
    {{0, 0, 1}, Face::Bottom}, // Top-Left     (Front-Left)
    {{0, 0, 0}, Face::Bottom}, // Bottom-Left  (Back-Left)

    // Left (-X)
    {{0, 0, 0}, Face::Left}, // Bottom-Left  (Back-Bottom)
    {{0, 0, 1}, Face::Left}, // Bottom-Right (Front-Bottom)
    {{0, 1, 1}, Face::Left}, // Top-Right    (Front-Top)
    {{0, 1, 1}, Face::Left}, // Top-Right    (Front-Top)
    {{0, 1, 0}, Face::Left}, // Top-Left     (Back-Top)
    {{0, 0, 0}, Face::Left}, // Bottom-Left  (Back-Bottom)

    // Right (+X)
    {{1, 0, 1}, Face::Right}, // Bottom-Left  (Front-Bottom)
    {{1, 0, 0}, Face::Right}, // Bottom-Right (Back-Bottom)
    {{1, 1, 0}, Face::Right}, // Top-Right    (Back-Top)
    {{1, 1, 0}, Face::Right}, // Top-Right    (Back-Top)
    {{1, 1, 1}, Face::Right}, // Top-Left     (Front-Top)
    {{1, 0, 1}, Face::Right}, // Bottom-Left  (Front-Bottom)
}};

std::array<bool, 6> GetShouldCull(const ChunkData &chunk, BlockRegistry &reg,
                                  const BlockPosition &pos) {
    constexpr glm::ivec3 offs[6] = {
        {0, 0, 1}, {0, 0, -1}, {0, 1, 0}, {0, -1, 0}, {-1, 0, 0}, {1, 0, 0},
    };

    std::array<bool, 6> shouldCull{};

    for (int i = 0; i < 6; ++i) {
        const glm::ivec3 neighbor = glm::ivec3(pos) + offs[i];

        if (neighbor.x < 0 || neighbor.x >= Chunk::Size || neighbor.y < 0 ||
            neighbor.y >= Chunk::Size || neighbor.z < 0 ||
            neighbor.z >= Chunk::Size) {

            shouldCull[i] = false;
            continue;
        }

        BlockPosition neighborPos(static_cast<uint8_t>(neighbor.x),
                                  static_cast<uint8_t>(neighbor.y),
                                  static_cast<uint8_t>(neighbor.z));

        BlockIndex idx = neighborPos.Idx();

        Block block = chunk.GetBlock(idx);
        shouldCull[i] = !reg.IsTransparent(block.id);
    }

    return shouldCull;
}

ChunkMeshData ChunkMesher::GenerateMesh(const ChunkData &chunk,
                                        BlockRegistry &reg) {
    ChunkMeshData mesh;
    mesh.position = chunk.position;

    if (chunk.isUniform && !reg.IsTransparent(chunk.uniformType)) {

        mesh.vertices.reserve(blockVertices.size());

        for (int face = 0; face != static_cast<int>(Face::Last); face++) {
            for (int faceVertexIndex = 0; faceVertexIndex < 6;
                 faceVertexIndex++) {
                int vertexIndex = faceVertexIndex + face * 6;
                const auto &ogVertex = blockVertices[vertexIndex];
                glm::vec3 newPos =
                    ogVertex.position * static_cast<float>(Chunk::Size);
                mesh.vertices.emplace_back(
                    ChunkMeshVertex{newPos, ogVertex.face});
            }
        }

        return mesh;
    }

    mesh.vertices.reserve(blockVertices.size() * 2048);

    for (size_t i = 0; i < chunk.blocks->size(); i++) {
        BlockIndex blockIndex = BlockIndex(i);
        if (!blockIndex.IsValid())
            continue;

        auto block = chunk.GetBlock(blockIndex);
        auto maybePos = blockIndex.Pos();

        if (reg.IsTransparent(block.id) || !maybePos.has_value())
            continue;

        auto pos = *maybePos;

        auto shouldCull = GetShouldCull(chunk, reg, pos);
        for (int face = 0; face != static_cast<int>(Face::Last); face++) {
            if (shouldCull[face])
                continue;

            for (int faceVertexIndex = 0; faceVertexIndex < 6;
                 faceVertexIndex++) {
                int vertexIndex = faceVertexIndex + face * 6;
                const auto &ogVertex = blockVertices[vertexIndex];
                glm::vec3 newPos = ogVertex.position + glm::vec3(pos);
                mesh.vertices.emplace_back(
                    ChunkMeshVertex{newPos, ogVertex.face});
            }
        }
    }

    return mesh;
}