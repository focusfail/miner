#pragma once

#include "Util/NonCopyable.hh"
#include "Util/NonMoveable.hh"

#include <cstddef>
#include <cstdint>

class GpuBuffer : private NonCopyable, private NonMovable {
public:
    GpuBuffer() = default;
    ~GpuBuffer();

    void Init();
    void Bind(uint32_t loc);

    void Allocate(size_t size);
    void Upload(size_t offset, size_t size, const void *data);

    uint32_t Handle() const { return m_Buffer; }
    uint32_t Capacity() const { return m_Capacity; }

private:
    uint32_t m_Buffer = 0;
    size_t m_Capacity = 0;
};