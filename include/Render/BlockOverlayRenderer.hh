#pragma once

#include "Render/Camera.hh"
#include "Render/ShaderProgram.hh"
#include "Util/NonCopyable.hh"
#include "Util/NonMoveable.hh"

#include <cstdint>
#include <vector>

struct BlockOverlay {
    glm::ivec3 blockPos;
    glm::vec4 color;
};

class BlockOverlayRenderer : private NonCopyable, private NonMovable {
  public:
    void Init();

    void DrawBox(const glm::ivec3 &worldBlockPos, const glm::vec4 &color);
    void DrawOutline(const glm::ivec3 &worldBlockPos, const glm::vec4 &color);
    void Render(Camera &cam);

  private:
    void RenderBox(Camera &cam);
    void RenderOutline(Camera &cam);

    uint32_t m_BoxVBO;
    uint32_t m_OutlineVBO;
    uint32_t m_VAO;
    ShaderProgram *m_Program;

    std::vector<BlockOverlay> m_Boxes;
    std::vector<BlockOverlay> m_Outlines;
};