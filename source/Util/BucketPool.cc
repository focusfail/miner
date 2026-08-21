#include "Util/BucketPool.hh"
#include <algorithm>
#include <optional>

void BucketPool::Init(std::vector<BucketDef> bucketDefs) {
    std::sort(
        bucketDefs.begin(), bucketDefs.end(),
        [](const BucketDef &a, const BucketDef &b) { return a.size < b.size; });

    m_Buckets.resize(bucketDefs.size());

    size_t offset = 0;
    for (size_t i = 0; i < m_Buckets.size(); i++) {
        auto &b = m_Buckets[i];
        b.slotSize = bucketDefs[i].size;
        b.baseOffset = offset;

        for (size_t j = 0; j < bucketDefs[i].count; j++) {
            b.freeSlots.push_back(j);
        }
        offset += b.slotSize * bucketDefs[i].count;
    }

    m_Capacity = offset;
}

std::optional<BucketPool::Slot> BucketPool::Allocate(size_t size) {
    int idx = PickBucket(size);
    if (idx < 0) return std::nullopt;

    auto &b = m_Buckets[idx];
    if (b.freeSlots.empty()) return std::nullopt;
    size_t slot = b.freeSlots.back();
    b.freeSlots.pop_back();

    return Slot{b.baseOffset + slot * b.slotSize, b.slotSize, idx,
                static_cast<int>(slot)};
}

void BucketPool::Free(Slot s) {
    m_Buckets[s.bucketIdx].freeSlots.push_back(s.slotIdx);
}

int BucketPool::PickBucket(size_t size) {
    for (size_t i = 0; i < m_Buckets.size(); i++) {
        if (m_Buckets[i].slotSize >= size) return static_cast<int>(i);
    }

    return -1;
}