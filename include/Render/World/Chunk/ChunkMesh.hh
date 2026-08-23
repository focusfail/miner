#pragma once

#include "Render/GpuPool.hh"
#include "World/Coordinates.hh"

#include <cstdint>
#include <glm/ext/vector_float3.hpp>

struct ChunkMeshVertex {
    glm::vec3 position;
    float light = 4.0f / 15.0f;
};

struct alignas(8) ChunkMeshFace {
    uint32_t face;
    uint32_t texId;
};

struct ChunkMeshChunk {
    glm::vec3 position;
    uint32_t faceOffset;
    uint32_t baseVertex;
    uint32_t isUniform;
    uint32_t padding[2];
};

struct ChunkMeshData {
    ChunkPosition position;
    std::vector<ChunkMeshVertex> vertices;
    std::vector<ChunkMeshFace> faces;
    ChunkMeshChunk chunk;
};

struct ChunkMesh {
    ChunkPosition position;
    GpuPool<ChunkMeshVertex>::Slot vertexSlot;
    GpuPool<ChunkMeshFace>::Slot faceSlot;
    GpuPool<ChunkMeshChunk>::Slot chunkSlot;
    bool active = false;
};