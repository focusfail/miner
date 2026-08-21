#pragma once

#include "Util/NonCopyable.hh"
#include "Util/NonMoveable.hh"

#include <optional>
#include <vector>

class BucketPool : private NonCopyable, private NonMovable {
public:
    struct Slot {
        size_t offset;
        size_t size;
        int bucketIdx;
        int slotIdx;
    };

    struct BucketDef {
        size_t size;
        size_t count;
    };

    void Init(std::vector<BucketDef> bucketDefs);

    std::optional<Slot> Allocate(size_t size);
    void Free(Slot slot);

    size_t Capacity() const { return m_Capacity; }

private:
    int PickBucket(size_t size);

    struct Bucket {
        size_t slotSize;
        size_t baseOffset;
        std::vector<size_t> freeSlots;
    };

    size_t m_Capacity = 0;
    std::vector<Bucket> m_Buckets;
};