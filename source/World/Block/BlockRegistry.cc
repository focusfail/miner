#include "World/Block/BlockRegistry.hh"
#include "World/Block/Block.hh"

void BlockRegistry::Register(const std::string &name, bool solid,
                             bool transparent) {
  m_Defs.emplace_back(m_Defs.size(), solid, transparent, name);
}

auto BlockRegistry::GetName(BlockID id) -> const std::string & {
  return m_Defs.at(id).name;
}

auto BlockRegistry::IsSolid(BlockID id) -> bool { return m_Defs.at(id).solid; }
auto BlockRegistry::IsTransparent(BlockID id) -> bool {
  return m_Defs.at(id).transparent;
}
