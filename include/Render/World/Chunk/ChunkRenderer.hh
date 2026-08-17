#pragma once

#include "Render/Camera.hh"
#include "Render/Shader.hh"
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

private:
  Shader m_Program;
  std::unordered_map<ChunkPosition, ChunkMesh> m_Meshes;
};