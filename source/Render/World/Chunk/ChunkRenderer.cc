#include "Render/World/Chunk/ChunkRenderer.hh"

void ChunkRenderer::Init() {
    m_Program.Load("~/pp/miner/assets/Shaders/Chunk.vert",
                   "~/pp/miner/assets/Shaders/Chunk.frag");

    assert(m_Program.IsValid());
}

void ChunkRenderer::UploadMesh(const ChunkMeshData &mesh) {
    auto &chunkMesh =
        m_Meshes[mesh.position]; // creates default ChunkMesh if new
    chunkMesh.position = mesh.position;
    chunkMesh.Upload(mesh);
}

void ChunkRenderer::RemoveMesh(const ChunkPosition &pos) {
    auto it = m_Meshes.find(pos);
    if (it == m_Meshes.end()) {
        return;
    }
    it->second.Destroy();
    m_Meshes.erase(it);
}

void ChunkRenderer::Render(const Camera &cam) {
    glm::mat4 proj =
        glm::perspective(glm::radians(75.0f), 1920.0f / 1080.0f, 0.1f, 1000.0f);
    glm::mat4 mvp = proj * cam.GetViewMatrix();

    m_Program.Use();
    m_Program.SetUniform(0, mvp);
    for (auto &[pos, mesh] : m_Meshes) {
        m_Program.SetUniform(1, mesh.position * CHUNK_SIZE);
        mesh.Render();
    }
}

void ChunkRenderer::Destroy() {
    for (auto &[pos, mesh] : m_Meshes) {
        mesh.Destroy();
    }
    m_Meshes.clear();
}
