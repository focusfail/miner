#pragma once

#include "Render/Camera.hh"
#include "Render/World/Chunk/ChunkMesher.hh"
#include "Render/World/Chunk/ChunkRenderer.hh"
#include "World/Chunk/ChunkManager.hh"

#include "Util/NonCopyable.hh"
#include "Util/NonMoveable.hh"

class World : private NonCopyable, private NonMovable {
public:
  World() = default;
  ~World() = default;

  void Init();
  void Update();
  void Render(const Camera &cam);
  void Destroy();

private:
  ChunkMesher m_ChunkMesher;
  ChunkManager m_ChunkManager;
  ChunkRenderer m_ChunkRenderer;
};