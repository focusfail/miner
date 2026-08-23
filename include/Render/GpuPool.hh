#pragma once

#include "Render/GpuBuffer.hh"
#include "Util/BucketPool.hh"

#include <cassert>
#include <optional>
#include <span>
#include <vector>

template <typename T> class GpuPool {
public:
    using Slot = BucketPool::Slot;
    using Bucket = BucketPool::BucketDef;

    void Init(std::vector<Bucket> buckets);
    void Bind(uint32_t loc = 0);

    std::optional<Slot> Allocate(uint32_t count);
    void Upload(Slot slot, std::span<const T> data);
    void Free(Slot slot);

    uint32_t Handle() const { return m_Buffer.Handle(); }

private:
    GpuBuffer m_Buffer;
    BucketPool m_Pool;
};

template <typename T> void GpuPool<T>::Init(std::vector<Bucket> buckets) {
    m_Pool.Init(std::move(buckets));

    m_Buffer.Init();
    m_Buffer.Allocate(m_Pool.Capacity());
}

template <typename T> void GpuPool<T>::Bind(uint32_t loc) {
    m_Buffer.Bind(loc);
}
template <typename T>
std::optional<typename GpuPool<T>::Slot> GpuPool<T>::Allocate(uint32_t count) {
    return m_Pool.Allocate(sizeof(T) * count);
}

template <typename T>
void GpuPool<T>::Upload(Slot slot, std::span<const T> data) {
    assert(slot.size >= data.size_bytes());

    m_Buffer.Upload(slot.offset, data.size_bytes(), data.data());
}

template <typename T> void GpuPool<T>::Free(Slot slot) { m_Pool.Free(slot); }