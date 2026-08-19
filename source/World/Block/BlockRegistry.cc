#include "World/Block/BlockRegistry.hh"
#include "World/Block/Block.hh"

void BlockRegistry::Register(const std::string &name, bool solid,
                             bool transparent) {
    m_Defs.emplace_back(m_Defs.size(), solid, transparent, name);
}

const std::string &BlockRegistry::GetName(BlockID id) {
    return m_Defs.at(id).name;
}

bool BlockRegistry::IsSolid(BlockID id) { return m_Defs.at(id).solid; }
bool BlockRegistry::IsTransparent(BlockID id) {
    return m_Defs.at(id).transparent;
}
