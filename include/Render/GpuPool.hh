#pragma once

#include "Render/GpuBuffer.hh"
#include "Util/BucketPool.hh"

#include <span>

template <typename T> class GpuPool {
public:
    using Slot = BucketPool::Slot;
    using Bucket = BucketPool::BucketDef;

    void Init(std::vector<Bucket> buckets);
    void Bind(uint32_t loc = 0);

    auto Allocate(uint32_t count);
    void Upload(Slot slot, std::span<const T> data);
    void Free(Slot slot);

    uint32_t Handle() const { return m_Buffer.Handle(); }

private:
    T m_Type;
    GpuBuffer m_Buffer;
    BucketPool m_Pool;
};
