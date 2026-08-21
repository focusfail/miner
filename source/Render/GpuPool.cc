#include "Render/GpuPool.hh"
#include <cassert>

template <typename T>
void GpuPool<T>::Init(std::vector<GpuPool::Bucket> buckets) {
    m_Pool.Init(buckets);

    m_Buffer.Init();
    m_Buffer.Allocate(m_Pool.Capacity());
}

template <typename T> void GpuPool<T>::Bind(uint32_t loc) {
    m_Buffer.Bind(loc);
}

template <typename T> auto GpuPool<T>::Allocate(uint32_t count) {
    return m_Pool.Allocate(sizeof(T) * count);
}

template <typename T>
void GpuPool<T>::Upload(GpuPool::Slot slot, std::span<const T> data) {
    assert(slot.size >= data.size_bytes());
    m_Buffer.Upload(slot.offset, data.size_bytes(), data.data());
}
template <typename T> void GpuPool<T>::Free(GpuPool::Slot slot) {
    m_Pool.Free(slot);
}