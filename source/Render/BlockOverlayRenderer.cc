#include "Render/BlockOverlayRenderer.hh"
#include "Assets/AssetsManager.hh"
#include "glm/ext/matrix_clip_space.hpp"
#include <glad/gl.h>

struct Vertex {
    glm::vec3 position;
    glm::vec4 color;
};

static const std::array<glm::vec3, 36> blockVertices = {
    {{0, 0, 1}, {1, 0, 1}, {1, 1, 1}, {1, 1, 1}, {0, 1, 1}, {0, 0, 1},
     {1, 0, 0}, {0, 0, 0}, {0, 1, 0}, {0, 1, 0}, {1, 1, 0}, {1, 0, 0},
     {0, 1, 1}, {1, 1, 1}, {1, 1, 0}, {1, 1, 0}, {0, 1, 0}, {0, 1, 1},
     {0, 0, 0}, {1, 0, 0}, {1, 0, 1}, {1, 0, 1}, {0, 0, 1}, {0, 0, 0},
     {0, 0, 0}, {0, 0, 1}, {0, 1, 1}, {0, 1, 1}, {0, 1, 0}, {0, 0, 0},
     {1, 0, 1}, {1, 0, 0}, {1, 1, 0}, {1, 1, 0}, {1, 1, 1}, {1, 0, 1}}};

static size_t s_BoxVBOCapacity = 1024;

void BlockOverlayRenderer::Init() {
    auto &assets = AssetsManager::GetInstance();
    m_Program = assets.GetShaderProgram("debug:basic");

    glCreateVertexArrays(1, &m_VAO);
    glCreateBuffers(1, &m_BoxVBO);

    glNamedBufferData(m_BoxVBO, sizeof(Vertex) * s_BoxVBOCapacity, nullptr,
                      GL_DYNAMIC_DRAW);

    glVertexArrayAttribFormat(m_VAO, 0, 3, GL_FLOAT, GL_FALSE,
                              offsetof(Vertex, position));
    glVertexArrayAttribFormat(m_VAO, 1, 4, GL_FLOAT, GL_FALSE,
                              offsetof(Vertex, color));

    glVertexArrayAttribBinding(m_VAO, 0, 0);
    glVertexArrayAttribBinding(m_VAO, 1, 0);

    glEnableVertexArrayAttrib(m_VAO, 0);
    glEnableVertexArrayAttrib(m_VAO, 1);

    glVertexArrayVertexBuffer(m_VAO, 0, m_BoxVBO, 0, sizeof(Vertex));
}

void BlockOverlayRenderer::DrawBox(const glm::ivec3 &pos,
                                   const glm::vec4 &color) {
    m_Boxes.emplace_back(pos, color);
}

void BlockOverlayRenderer::Render(Camera &cam) {
    if (m_Boxes.empty())
        return;

    m_Program->Bind();
    glBindVertexArray(m_VAO);

    RenderBox(cam);

    m_Boxes.clear();
}

void BlockOverlayRenderer::RenderBox(Camera &cam) {
    std::vector<Vertex> verts;
    verts.reserve(m_Boxes.size() * 36);

    const float offset = -0.005f;
    const float scale = 1.01f;

    for (const auto &box : m_Boxes) {
        for (const auto &pos : blockVertices) {
            glm::vec3 expandedPos = (pos * scale) + glm::vec3(offset);
            verts.push_back(
                Vertex{expandedPos + glm::vec3(box.blockPos), box.color});
        }
    }

    if (verts.size() > s_BoxVBOCapacity) {
        s_BoxVBOCapacity = verts.size() * 2;
        glNamedBufferData(m_BoxVBO, sizeof(Vertex) * s_BoxVBOCapacity,
                          verts.data(), GL_DYNAMIC_DRAW);
    } else {
        glNamedBufferSubData(m_BoxVBO, 0, sizeof(Vertex) * verts.size(),
                             verts.data());
    }

    glm::mat4 proj =
        glm::perspective(glm::radians(75.0f), 1920.0f / 1080.0f, 0.1f, 1000.0f);
    m_Program->SetUniform(0, proj * cam.GetViewMatrix());

    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(verts.size()));
}