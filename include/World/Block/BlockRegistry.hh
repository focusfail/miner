#pragma once

#include "Util/NonCopyable.hh"
#include "Util/NonMoveable.hh"
#include "World/Block/Block.hh"
#include <vector>

class BlockRegistry : private NonCopyable, private NonMovable {
public:
  void Register(const std::string &name, bool solid, bool transparent);

  auto GetName(BlockID id) -> const std::string &;
  auto IsSolid(BlockID id) -> bool;
  auto IsTransparent(BlockID id) -> bool;

private:
  std::vector<BlockDefinition> m_Defs;
};