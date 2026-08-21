#pragma once

#include "Util/NonCopyable.hh"
#include "Util/NonMoveable.hh"

#include <cstdint>
#include <optional>
#include <vector>

class BucketPool : private NonCopyable, private NonMovable {
    struct Slot {
        uint32_t offset;
        uint32_t size;
        int bucketIdx;
    };

    BucketPool(std::vector<uint32_t> bucketSizes, uint32_t bucketSlots);

    std::optional<Slot> Allocate(uint32_t size);
    void Free(Slot slot);

private:
    int PickBucket(uint32_t size);

    struct Bucket {
        uint32_t slotSize;
        uint32_t baseOffset;
        std::vector<uint32_t> freeSlots;
    };

    std::vector<uint32_t> m_BucketSizes;
    std::vector<Bucket> m_Buckets;
};