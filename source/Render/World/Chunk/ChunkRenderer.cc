#include "Render/World/Chunk/ChunkRenderer.hh"
#include "Assets/AssetsManager.hh"
#include "Render/World/Chunk/ChunkMesh.hh"
#include "spdlog/spdlog.h"
#include <glad/gl.h>

void ChunkRenderer::Init() {
    auto &assets = AssetsManager::GetInstance();

    m_Program = assets.GetShaderProgram("chunk:v2");

    assert(m_Program->IsValid());

    glCreateVertexArrays(1, &m_DummyVAO);
    m_VertexPool.Init({{sizeof(ChunkMeshVertex) * 74'000, 32}});
    m_FacePool.Init({{sizeof(ChunkMeshFace) * 30'000, 32}});
    m_ChunkPool.Init({{sizeof(ChunkMeshChunk), 32}});
    m_CmdBuffer.Init();
    m_CmdBuffer.Allocate(sizeof(DrawCmd) * 32);
}

void ChunkRenderer::UploadMesh(const ChunkMeshData &mesh) {
    if (mesh.vertices.empty()) {
        spdlog::info("[ChunkRenderer] Tried to render empty mesh");
        return;
    }

    auto &chunkMesh = m_Meshes[mesh.position];
    chunkMesh.position = mesh.position;
    if (!chunkMesh.active) {
        if (auto slot = m_VertexPool.Allocate(mesh.vertices.size())) {
            m_VertexPool.Upload(*slot, mesh.vertices);
            chunkMesh.vertexSlot = *slot;
        }
        if (auto slot = m_FacePool.Allocate(mesh.faces.size())) {
            m_FacePool.Upload(*slot, mesh.faces);
            chunkMesh.faceSlot = *slot;
        }
        if (auto slot = m_ChunkPool.Allocate(1)) {
            ChunkMeshChunk c[] = {mesh.chunk};
            c[0].faceOffset = static_cast<uint32_t>(chunkMesh.faceSlot.offset / sizeof(ChunkMeshFace));
            c[0].baseVertex = static_cast<uint32_t>(chunkMesh.vertexSlot.offset / sizeof(ChunkMeshVertex));

            m_ChunkPool.Upload(*slot, c);
            chunkMesh.chunkSlot = *slot;
        }
        chunkMesh.active = true;
    } else {
        m_VertexPool.Upload(chunkMesh.vertexSlot, mesh.vertices);
        m_FacePool.Upload(chunkMesh.faceSlot, mesh.faces);
        ChunkMeshChunk c[] = {mesh.chunk};
        c[0].faceOffset = static_cast<uint32_t>(chunkMesh.faceSlot.offset / sizeof(ChunkMeshFace));
        c[0].baseVertex = static_cast<uint32_t>(chunkMesh.vertexSlot.offset / sizeof(ChunkMeshVertex));

        m_ChunkPool.Upload(chunkMesh.chunkSlot, c);
    }

    m_IsDirty = true;
}

void ChunkRenderer::RemoveMesh(const ChunkPosition &pos) {
    auto it = m_Meshes.find(pos);
    if (it == m_Meshes.end()) {
        return;
    }
    m_Meshes.erase(it);
    m_IsDirty = true;
}

void ChunkRenderer::Render(const Camera &cam) {
    if (m_IsDirty) {
        m_DrawCmds.clear();
        for (const auto &[pos, mesh] : m_Meshes) {
            if (!mesh.active) continue;

            m_DrawCmds.push_back({
                .count = static_cast<uint32_t>(mesh.vertexSlot.size / sizeof(ChunkMeshVertex)),
                .instanceCount = 1,
                .firstIndex = static_cast<uint32_t>(mesh.vertexSlot.offset / sizeof(ChunkMeshVertex)),
                .baseInstance = static_cast<uint32_t>(mesh.chunkSlot.offset / sizeof(ChunkMeshChunk)),
            });
        }
        m_CmdBuffer.Upload(0, m_DrawCmds.size() * sizeof(DrawCmd), m_DrawCmds.data());
    }

    glm::mat4 proj = glm::perspective(glm::radians(75.0f), 1920.0f / 1080.0f, 0.1f, 1000.0f);
    glm::mat4 vp = proj * cam.GetViewMatrix();

    m_Program->Bind();
    glBindVertexArray(m_DummyVAO);
    m_Texture->Bind(0);
    m_Overlays->Bind(1);
    m_ChunkPool.Bind(0);
    m_FacePool.Bind(1);
    m_VertexPool.Bind(2);
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, m_CmdBuffer.Handle());
    m_Program->SetUniform(0, vp);

    if (!m_DrawCmds.empty()) {
        glMultiDrawArraysIndirect(
            GL_TRIANGLES, reinterpret_cast<const void *>(0), static_cast<GLsizei>(m_DrawCmds.size()), sizeof(DrawCmd));
    }
}

void ChunkRenderer::Destroy() { m_Meshes.clear(); }
