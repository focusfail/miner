#pragma once

#include <cstdint>
#include <string>

using BlockID = uint16_t;

struct Block {
    BlockID id = 0;
    struct {
        uint8_t breakStage : 4 = 0;
        uint8_t lightEmit : 4 = 0;
        uint8_t lightLv : 4 = 1;
    };
};

struct BlockInfo {
    BlockID id;
    int texId;
    bool solid;
    bool transparent;
    uint8_t breakStage;
    uint8_t lightEmit;
    uint8_t lightLv;
};

Block BlockFromID(BlockID id);