#include "Assets/AssetsManager.hh"
#include "vfspp/IFile.h"

#include <cstdint>
#include <memory>
#include <type_traits>
#include <vector>

#include <spdlog/spdlog.h>
#include <vfspp/VFS.h>

std::string assetName2Path(const std::string &root, const std::string &name,
                           const std::string &ext) {
    auto copy = name;
    for (char &c : copy) {
        if (c == ':') {
            c = '/';
        }
    }

    return root + copy + ext;
}

struct AssetsManager::FileSystem {
    void MountNative(const std::string &realDir, const std::string &virtDir) {
        vfs = std::make_unique<vfspp::VirtualFileSystem>();

        if (!vfs->CreateFileSystem<vfspp::NativeFileSystem>(virtDir, realDir)) {
            spdlog::error("[FileSystem] Failed to mount game assets from: {}",
                          realDir);
        }
    }

    void MountArchive(const std::string &archive, const std::string &virtDir) {
        vfs = std::make_unique<vfspp::VirtualFileSystem>();

        if (!vfs->CreateFileSystem<vfspp::ZipFileSystem>(virtDir, archive)) {
            spdlog::error("[FileSystem] Failed to mount game assets from: {}",
                          archive);
            exit(1);
        }
    }

    bool Exists(const std::string &path) { return vfs->IsFileExists(path); }

    std::vector<uint8_t> Read(const std::string &path) {
        if (auto f = vfs->OpenFile(path, vfspp::IFile::FileMode::Read)) {
            if (f->Size() == 0) {
                spdlog::warn("[FileSystem] File is empty");
                return {};
            }
            std::vector<uint8_t> buf(f->Size());
            f->Read(buf);
            return buf;
        }
        spdlog::error("[FileSystem] Failed to open file for reading: {}", path);
        return {};
    }

    std::unique_ptr<vfspp::VirtualFileSystem> vfs;
};

AssetsManager::AssetsManager() = default;
AssetsManager::~AssetsManager() = default;

void AssetsManager::Init() {
    m_FS = std::make_unique<FileSystem>();

#ifdef RELEASE_BUILD
    m_FS->MountArchive("assets.zip", "/");
#else
    m_FS->MountNative(GAME_ASSET_PATH, "/");
#endif

    spdlog::info("[AssetsManager] Mounted filesystem {}", GAME_ASSET_PATH);
    for (auto &f : m_FS->vfs->ListAllFiles()) {
        spdlog::info("{}", f);
    }
}

ShaderProgram *AssetsManager::GetShaderProgram(const std::string &name) {
    auto it = m_Shaders.find(name);
    if (it != m_Shaders.end()) {
        return it->second.get();
    }

    auto vertPath = assetName2Path("/shaders/", name, ".vert");
    auto fragPath = assetName2Path("/shaders/", name, ".frag");
    if (!m_FS->Exists(vertPath) || !m_FS->Exists(fragPath)) {
        spdlog::error("[AssetsManager] File does not exist: {}", name);
        exit(1);
    }

    auto vertData = m_FS->Read(vertPath);
    auto fragData = m_FS->Read(fragPath);

    if (vertData.empty() || fragData.empty()) {
        spdlog::error("[AssetsManager] Failed to load shader: {}", name);
        exit(1);
    }

    auto shader = std::make_unique<ShaderProgram>();
    shader->LoadStr(std::string(vertData.begin(), vertData.end()),
                    std::string(fragData.begin(), fragData.end()));

    ShaderProgram *p = shader.get();
    m_Shaders[name] = std::move(shader);

    spdlog::info("[AssetsManager] Loaded shader: {}", name);
    return p;
}

Texture *AssetsManager::GetTexture(const std::string &name) {
    auto it = m_Textures.find(name);
    if (it != m_Textures.end()) {
        return it->second.get();
    }

    auto path = assetName2Path("/textures/", name, ".png");
    if (!m_FS->Exists(path)) {
        spdlog::error("[AssetsManager] File does not exist: {}", name);
        exit(1);
    }

    auto texData = m_FS->Read(path);
    if (texData.empty()) {
        spdlog::error("[AssetsManager] Failed to load texture: {}", name);
        exit(1);
    }

    auto texture = std::make_unique<Texture>();
    texture->Load(texData);

    Texture *p = texture.get();
    m_Textures[name] = std::move(texture);

    spdlog::info("[AssetsManager] Loaded texture: {}", name);
    return p;
}

std::vector<uint8_t> AssetsManager::GetData(const std::string &path) {
    auto fullPath = assetName2Path("", path, "");
    if (m_FS->Exists(fullPath)) {
        auto buf = m_FS->Read(fullPath);
        return buf;
    }

    return {};
}