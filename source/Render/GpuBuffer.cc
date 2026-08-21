#include "Render/GpuBuffer.hh"

#include <cassert>
#include <glad/gl.h>

GpuBuffer::~GpuBuffer() {
    if (m_Buffer != 0) {
        glDeleteBuffers(1, &m_Buffer);
    }
}

void GpuBuffer::Init() { glCreateBuffers(1, &m_Buffer); }
void GpuBuffer::Bind(uint32_t loc) {
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, loc, m_Buffer);
}

void GpuBuffer::Allocate(size_t size) {
    assert(m_Buffer != 0);

    glNamedBufferStorage(m_Buffer, size, nullptr, GL_DYNAMIC_STORAGE_BIT);
    m_Capacity = size;
}

void GpuBuffer::Upload(size_t offset, size_t size, const void *data) {
    assert(offset + size <= m_Capacity);
    glNamedBufferSubData(m_Buffer, offset, size, data);
}