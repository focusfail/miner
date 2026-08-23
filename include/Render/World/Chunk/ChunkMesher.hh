#pragma once

#include "Render/World/Chunk/ChunkMesh.hh"
#include "World/Chunk/Chunk.hh"

class ChunkMesher {
public:
    ChunkMesher() = default;
    ~ChunkMesher() = default;

    ChunkMeshData GenerateMesh(const Chunk &chunk, std::array<Chunk *, 6> &nbChunks);
};