#pragma once

#include "Util/NonCopyable.hh"
#include "Util/NonMoveable.hh"
#include "World/Block/Block.hh"
#include <vector>

class BlockRegistry : private NonCopyable, private NonMovable {
  public:
    void Register(const std::string &name, bool solid, bool transparent,
                  int texId);

    const std::string &GetName(BlockID id);
    bool IsSolid(BlockID id);
    bool IsTransparent(BlockID id);
    int GetTexture(BlockID id);

  private:
    std::vector<BlockDefinition> m_Defs;
};