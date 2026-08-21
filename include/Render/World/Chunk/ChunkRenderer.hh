#pragma once

#include "Render/Camera.hh"
#include "Render/GpuPool.hh"
#include "Render/ShaderProgram.hh"
#include "Render/TextureArray.hh"
#include "Render/World/Chunk/ChunkMesh.hh"
#include "Util/NonCopyable.hh"
#include "Util/NonMoveable.hh"
#include "World/Coordinates.hh"

#include <glm/gtx/hash.hpp>
#include <unordered_map>

class ChunkRenderer : private NonCopyable, private NonMovable {
public:
    void Init();
    void UploadMesh(const ChunkMeshData &mesh);
    void RemoveMesh(const ChunkPosition &pos);
    void Render(const Camera &cam);
    void Destroy();

    void SetTextureArray(TextureArray *ta) { m_Texture = ta; }

private:
    ShaderProgram *m_Program;
    TextureArray *m_Texture;
    std::unordered_map<ChunkPosition, ChunkMesh> m_Meshes;
};