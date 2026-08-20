#include "World/Block/BlockRegistry.hh"
#include "World/Block/Block.hh"

void BlockRegistry::Register(const std::string &name, bool solid,
                             bool transparent, int texId) {
    m_Defs.emplace_back(m_Defs.size(), texId, solid, transparent, name);
}

const std::string &BlockRegistry::GetName(BlockID id) {
    return m_Defs.at(id).name;
}

bool BlockRegistry::IsSolid(BlockID id) { return m_Defs.at(id).solid; }
bool BlockRegistry::IsTransparent(BlockID id) {
    return m_Defs.at(id).transparent;
}

int BlockRegistry::GetTexture(BlockID id) { return m_Defs.at(id).texId; }