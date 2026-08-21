#include "Util/BucketPool.hh"
#include <optional>

BucketPool::BucketPool(std::vector<uint32_t> sizes, uint32_t slots) {
    m_Buckets.resize(sizes.size());

    uint32_t offset = 0;
    for (size_t i = 0; i < m_BucketSizes.size(); i++) {
        auto &b = m_Buckets[i];
        b.slotSize = sizes[i];
        b.baseOffset = offset;

        for (uint32_t j = 0; j < slots; j++) {
            b.freeSlots.push_back(j);
        }
        offset += b.slotSize * slots;
    }
}

std::optional<BucketPool::Slot> BucketPool::Allocate(uint32_t size) {
    int idx = PickBucket(size);
    if (idx < 0) return std::nullopt;

    auto &b = m_Buckets[idx];
    if (b.freeSlots.empty()) return std::nullopt;
    uint32_t slot = b.freeSlots.back();
    b.freeSlots.pop_back();

    return Slot{b.baseOffset + slot * b.slotSize, b.slotSize, idx};
}

void BucketPool::Free(Slot s) {
    m_Buckets[s.bucketIdx].freeSlots.push_back(
        (s.offset - m_Buckets[s.bucketIdx].baseOffset) /
        m_Buckets[s.bucketIdx].slotSize);
}

int BucketPool::PickBucket(uint32_t size) {
    for (size_t i = 0; i < m_BucketSizes.size(); i++) {
        if (m_BucketSizes[i] >= size) return static_cast<int>(i);
    }

    return -1;
}