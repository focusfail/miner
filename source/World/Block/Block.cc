#include "World/Block/Block.hh"
auto BlockFromID(BlockID id) -> Block { return {{0, 0}, id}; }