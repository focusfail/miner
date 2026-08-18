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
  BlockID id;
};

auto BlockFromID(BlockID id) -> Block;