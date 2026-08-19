#pragma once

#include "Render/Camera.hh"
#include "Render/ShaderProgram.hh"
#include <cstdint>

struct DebugVertex {
    glm::vec3 position;
    glm::vec4 color;
};

struct DebugLine {
    glm::vec3 start;
    glm::vec3 end;
    glm::vec4 color;
    float timeout = 0.0f;
    float timer = 0.0f;
};

class DebugRenderer {
  public:
    void Init();

    void DrawLine(glm::vec3 start, glm::vec3 end, float timeout);
    void Render(Camera &cam, float dt);

  private:
    ShaderProgram *m_Program;
    uint32_t m_VAO;
    uint32_t m_VBO;
    size_t m_MaxSize;
    std::vector<DebugLine> m_Lines;
};