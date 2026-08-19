#pragma once

#include <cstdint>
#include <string>

using BlockID = uint16_t;

struct BlockDefinition {
    BlockID id;

    bool solid;
    bool transparent;

    std::string name;
};

struct Block {
    struct {
        uint8_t breakStage : 4 = 0;
        uint8_t dummy : 4 = 0;
    };
    BlockID id = 0;
};

auto BlockFromID(BlockID id) -> Block;