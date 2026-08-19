#include "Render/Debug/DebugRenderer.hh"
#include "Assets/AssetsManager.hh"
#include "glm/ext/matrix_clip_space.hpp"

#include <glad/gl.h>

void DebugRenderer::Init() {
    auto &assets = AssetsManager::GetInstance();
    m_Program = assets.GetShader("shaders/debug/basic");

    assert(m_Program->IsValid());
    m_MaxSize = 2 * 1024;
    glCreateBuffers(1, &m_VBO);
    glCreateVertexArrays(1, &m_VAO);

    glNamedBufferData(m_VBO, sizeof(DebugVertex) * m_MaxSize, nullptr,
                      GL_STATIC_DRAW);
    glVertexArrayAttribFormat(m_VAO, 0, 3, GL_FLOAT, GL_FALSE,
                              offsetof(DebugVertex, position));
    glVertexArrayAttribFormat(m_VAO, 1, 4, GL_FLOAT, GL_FALSE,
                              offsetof(DebugVertex, color));

    glVertexArrayAttribBinding(m_VAO, 0, 0);
    glVertexArrayAttribBinding(m_VAO, 1, 0);

    glEnableVertexArrayAttrib(m_VAO, 0);
    glEnableVertexArrayAttrib(m_VAO, 1);
    glVertexArrayVertexBuffer(m_VAO, 0, m_VBO, 0, sizeof(DebugVertex));
}

void DebugRenderer::DrawLine(glm::vec3 start, glm::vec3 end, float timeout) {
    m_Lines.emplace_back(start, end, glm::vec4(1.0f), timeout, 0.0f);
}

void DebugRenderer::Render(Camera &cam, float dt) {
    glm::mat4 proj =
        glm::perspective(glm::radians(75.0f), 1920.0f / 1080.0f, 0.1f, 1000.0f);

    std::vector<DebugVertex> verts;
    std::vector<DebugLine>::iterator it;
    for (it = m_Lines.begin(); it != m_Lines.end();) {
        verts.push_back({it->start, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)});
        verts.push_back({it->end, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)});

        if (it->timeout != 0.0f && it->timer > it->timeout) {
            it = m_Lines.erase(it);
        } else {
            it->timer += dt;
            ++it;
        }
    }

    glNamedBufferSubData(m_VBO, 0, sizeof(verts[0]) * verts.size(),
                         verts.data());
    m_Program->Use();
    m_Program->SetUniform(0, proj * cam.GetViewMatrix());
    glBindVertexArray(m_VAO);
    glDrawArrays(GL_LINES, 0, verts.size());
}