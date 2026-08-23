#pragma once

#include <cstdint>
#include <string>

using BlockID = uint16_t;

struct BlockDefinition {
    BlockID id;

    int texId;

    bool solid;
    bool transparent;

    std::string name;
};

struct Block {
    BlockID id = 0;
    struct {
        uint8_t breakStage : 4 = 0;
        uint8_t lightEmit : 4 = 0;
        uint8_t lightLv : 4 = 1;
    };
};

auto BlockFromID(BlockID id) -> Block;