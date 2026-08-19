#include "Assets/AssetsManager.hh"

#include <memory>
#include <type_traits>
#include <vector>

#include <spdlog/spdlog.h>
#include <vfspp/VFS.h>

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

    spdlog::info("[AssetsManager] Mounted filesystem");
}

Shader *AssetsManager::GetShader(const std::string &name) {
    auto it = m_Shaders.find(name);
    if (it != m_Shaders.end()) {
        return it->second.get();
    }

    if (!m_FS->Exists("/" + name + ".vert") ||
        !m_FS->Exists("/" + name + ".frag")) {
        spdlog::error("[AssetsManager] Failed to retrieve shader: {}", name);
        exit(1);
    }

    auto vBuf = m_FS->Read("/" + name + ".vert");
    auto fBuf = m_FS->Read("/" + name + ".frag");

    for (auto &file : m_FS->vfs->ListAllFiles()) {
        spdlog::info("{}", file);
    }
    spdlog::info("{}", std::string(vBuf.begin(), vBuf.end()));

    if (vBuf.empty() || fBuf.empty()) {
        spdlog::error("[AssetsManager] Failed to load shader: {}", name);
        exit(1);
    }

    auto shader = std::make_unique<Shader>();
    shader->LoadStr(std::string(vBuf.begin(), vBuf.end()),
                    std::string(fBuf.begin(), fBuf.end()));

    spdlog::info("[AssetsManager] Loaded shader: {}", name);
    Shader *p = shader.get();

    m_Shaders[name] = std::move(shader);
    return p;
}