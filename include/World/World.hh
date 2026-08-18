#pragma once

#include "Render/Camera.hh"
#include "Render/World/Chunk/ChunkMesher.hh"
#include "Render/World/Chunk/ChunkRenderer.hh"
#include "World/Chunk/Chunk.hh"
#include "World/Chunk/ChunkManager.hh"

#include "Util/NonCopyable.hh"
#include "Util/NonMoveable.hh"
#include "World/Coordinates.hh"
#include <optional>

class World : private NonCopyable, private NonMovable {
public:
  World() = default;
  ~World() = default;

  void Init();
  void Update();
  void Render(const Camera &cam);
  void Destroy();

  auto TryGetChunkDataByPosition(const ChunkPosition &pos)
      -> std::optional<ChunkData *>;

private:
  ChunkMesher m_ChunkMesher;
  ChunkManager m_ChunkManager;
  ChunkRenderer m_ChunkRenderer;
};