#include "Render/World/Chunk/ChunkMesher.hh"
#include "Render/World/Chunk/ChunkMesh.hh"
#include "World/Coordinates.hh"

static const std::array<ChunkMeshVertex, 36> blockVertices = {{
    //  Front (+Z)
    {{0, 0, 1}, Face::Front}, // bottom left
    {{1, 0, 1}, Face::Front}, // bottom right
    {{1, 1, 1}, Face::Front}, // top right
    {{1, 1, 1}, Face::Front}, // top right
    {{0, 1, 1}, Face::Front}, // top left
    {{0, 0, 1}, Face::Front}, // bottom left

    //  Back (Z)
    {{0, 0, 0}, Face::Back}, // bottom left
    {{0, 1, 0}, Face::Back}, // top left
    {{1, 1, 0}, Face::Back}, // top right
    {{1, 1, 0}, Face::Back}, // top right
    {{1, 0, 0}, Face::Back}, // bottom right
    {{0, 0, 0}, Face::Back}, // bottom left

    //  Top (+Y)
    {{0, 1, 1}, Face::Top}, // front left
    {{1, 1, 1}, Face::Top}, // front right
    {{1, 1, 0}, Face::Top}, // back right
    {{1, 1, 0}, Face::Top}, // back right
    {{0, 1, 0}, Face::Top}, // back left
    {{0, 1, 1}, Face::Top}, // front left

    //  Bottom (Y)
    {{0, 0, 0}, Face::Bottom}, // back left
    {{1, 0, 0}, Face::Bottom}, // back right
    {{1, 0, 1}, Face::Bottom}, // front right
    {{1, 0, 1}, Face::Bottom}, // front right
    {{0, 0, 1}, Face::Bottom}, // front left
    {{0, 0, 0}, Face::Bottom}, // back left

    //  Left (X)
    {{0, 0, 0}, Face::Left}, // back bottom
    {{0, 0, 1}, Face::Left}, // front bottom
    {{0, 1, 1}, Face::Left}, // front top
    {{0, 1, 1}, Face::Left}, // front top
    {{0, 1, 0}, Face::Left}, // back top
    {{0, 0, 0}, Face::Left}, // back bottom

    //  Right (+X)
    {{1, 0, 0}, Face::Right}, // back bottom
    {{1, 1, 0}, Face::Right}, // back top
    {{1, 1, 1}, Face::Right}, // front top
    {{1, 1, 1}, Face::Right}, // front top
    {{1, 0, 1}, Face::Right}, // front bottom
    {{1, 0, 0}, Face::Right}, // back bottom
}};

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
                    ogVertex.position * static_cast<float>(CHUNK_SIZE);
                mesh.vertices.emplace_back(
                    ChunkMeshVertex{newPos, ogVertex.face});
            }
        }
    } else {
        mesh.vertices.reserve(blockVertices.size() * 2048);

        for (size_t blockIndex = 0; blockIndex < chunk.blocks->size();
             blockIndex++) {
            auto block = chunk.GetBlock(blockIndex);
            auto pos = BlockIdx2Pos(blockIndex);

            if (reg.IsTransparent(block.id))
                continue;

            for (int face = 0; face != static_cast<int>(Face::Last); face++) {
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
    }

    return mesh;
}