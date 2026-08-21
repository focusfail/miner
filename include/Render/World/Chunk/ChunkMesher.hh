#pragma once

#include "Render/World/Chunk/ChunkMesh.hh"
#include "World/Block/BlockRegistry.hh"
#include "World/Chunk/Chunk.hh"

class ChunkMesher {
public:
    ChunkMesher() = default;
    ~ChunkMesher() = default;

    ChunkMeshData GenerateMesh(const ChunkInfo &chunk, BlockRegistry &reg);
};