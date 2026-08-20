#include "Render/World/Chunk/ChunkMesh.hh"
#include <cstddef>
#include <glad/gl.h>
#include <spdlog/spdlog.h>

void ChunkMesh::Upload(const ChunkMeshData &mesh) {
    if (this->vao == 0) {
        glCreateVertexArrays(1, &this->vao);

        glVertexArrayAttribFormat(this->vao, 0, 3, GL_FLOAT, GL_FALSE,
                                  offsetof(ChunkMeshVertex, position));
        glVertexArrayAttribFormat(this->vao, 1, 1, GL_UNSIGNED_BYTE, GL_FALSE,
                                  offsetof(ChunkMeshVertex, face));
        glVertexArrayAttribFormat(this->vao, 2, 1, GL_INT, GL_FALSE,
                                  offsetof(ChunkMeshVertex, texId));

        glVertexArrayAttribBinding(this->vao, 0, 0);
        glVertexArrayAttribBinding(this->vao, 1, 0);
        glVertexArrayAttribBinding(this->vao, 2, 0);
        glEnableVertexArrayAttrib(this->vao, 0);
        glEnableVertexArrayAttrib(this->vao, 1);
        glEnableVertexArrayAttrib(this->vao, 2);
    }

    if (this->vbo == 0) {
        glCreateBuffers(1, &this->vbo);
    }

    // FIX: dont reallocate buffer every time - fine for now
    glNamedBufferData(this->vbo,
                      mesh.vertices.size() * sizeof(mesh.vertices[0]),
                      mesh.vertices.data(), GL_STATIC_DRAW);
    this->numVertices = mesh.vertices.size();
    glVertexArrayVertexBuffer(this->vao, 0, this->vbo, 0,
                              sizeof(mesh.vertices[0]));
}

void ChunkMesh::Destroy() {
    if (this->vbo != 0) {
        glDeleteBuffers(1, &this->vbo);
    }
    if (this->vao != 0) {
        glDeleteVertexArrays(1, &this->vao);
    }
}

void ChunkMesh::Render() {
    if (this->numVertices == 0)
        return;
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, this->numVertices);
}