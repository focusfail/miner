#pragma once

#include "Util/NonCopyable.hh"
#include "Util/NonMoveable.hh"
#include "World/Block/Block.hh"
#include <string>
#include <unordered_map>
#include <vector>

struct BlockRegistryEntry {
    BlockID id;
    int texId;
    bool solid;
    bool transparent;
    std::string name;
};

class BlockRegistry : private NonCopyable, private NonMovable {
public:
    static BlockRegistry &GetInstance() {
        static BlockRegistry Instance;
        return Instance;
    }

    void Register(const std::string &name, bool solid, bool transparent, int texId);

    const std::string &GetName(BlockID id);
    bool IsSolid(BlockID id) const;
    bool IsTransparent(BlockID id) const;
    int GetTexture(BlockID id) const;
    Block MakeBlock(BlockID id) const;
    BlockID GetBlockIDByName(const std::string &name);
    const BlockRegistryEntry &GetEntry(BlockID id) const;

private:
    std::unordered_map<std::string, size_t> m_NameToIdx;
    std::vector<BlockRegistryEntry> m_Defs;
};