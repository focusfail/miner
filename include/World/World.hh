#pragma once

#include "Render/Camera.hh"
#include "Render/TextureArray.hh"
#include "Render/World/Chunk/ChunkMesher.hh"
#include "Render/World/Chunk/ChunkRenderer.hh"
#include "World/Block/Block.hh"
#include "World/Block/BlockRegistry.hh"
#include "World/Chunk/ChunkManager.hh"

#include "Util/NonCopyable.hh"
#include "Util/NonMoveable.hh"
#include "World/Coordinates.hh"
#include <optional>

struct HitResult {
    ChunkPosition chunkPos;
    BlockPosition blockPosition;
    Block block;
    glm::vec3 position;
    glm::vec3 normal;
};

class World : private NonCopyable, private NonMovable {
public:
    World() = default;
    ~World() = default;

    void Init();
    void Update();
    void Render(const Camera &cam);
    void Destroy();

    std::optional<HitResult> CastRay(glm::vec3 start, glm::vec3 end);
    bool IsChunkLoaded(const ChunkPosition &pos);
    Chunk *TryGetChunk(const ChunkPosition &pos);
    Chunk *TryGetChunk(glm::vec3 pos);
    bool IsBlockSolidAt(const glm::vec3 &pos);
    void PropagateLight(const ChunkPosition &chunkPos);

    BlockRegistry &GetBlockRegistry() { return m_BlockRegistry; }

private:
    ChunkMesher m_ChunkMesher;
    ChunkManager m_ChunkManager;
    ChunkRenderer m_ChunkRenderer;
    BlockRegistry m_BlockRegistry;
    TextureArray m_BlockTextures;
};