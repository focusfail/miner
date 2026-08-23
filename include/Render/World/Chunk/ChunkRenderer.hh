#pragma once

#include "Render/Camera.hh"
#include "Render/GpuBuffer.hh"
#include "Render/GpuPool.hh"
#include "Render/ShaderProgram.hh"
#include "Render/TextureArray.hh"
#include "Render/World/Chunk/ChunkMesh.hh"
#include "Util/NonCopyable.hh"
#include "Util/NonMoveable.hh"
#include "World/Coordinates.hh"

#include <glm/gtx/hash.hpp>
#include <unordered_map>

struct DrawCmd {
    uint32_t count;
    uint32_t instanceCount;
    uint32_t firstIndex;
    uint32_t baseInstance;
};

class ChunkRenderer : private NonCopyable, private NonMovable {
public:
    void Init();
    void UploadMesh(const ChunkMeshData &mesh);
    void RemoveMesh(const ChunkPosition &pos);
    void Render(const Camera &cam);
    void Destroy();

    void SetTextureArrays(TextureArray *blocks, TextureArray *overlays) {
        m_Texture = blocks;
        m_Overlays = overlays;
    }

private:
    ShaderProgram *m_Program;
    TextureArray *m_Texture;
    TextureArray *m_Overlays;
    GpuPool<ChunkMeshVertex> m_VertexPool;
    GpuPool<ChunkMeshFace> m_FacePool;
    GpuPool<ChunkMeshChunk> m_ChunkPool;
    GpuBuffer m_CmdBuffer;
    uint32_t m_DummyVAO = 0;
    std::vector<DrawCmd> m_DrawCmds;
    std::unordered_map<ChunkPosition, ChunkMesh> m_Meshes;
};