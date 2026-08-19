#pragma once

#include <Render/Shader.hh>
#include <memory>
#include <string>
#include <unordered_map>

class AssetsManager {
  public:
    static AssetsManager &GetInstance() {
        static AssetsManager Instance;
        return Instance;
    }

    void Init();

    auto GetShader(const std::string &name) -> Shader &;

  private:
    AssetsManager();
    ~AssetsManager();
    struct FileSystem;

    std::unordered_map<std::string, Shader> m_Shaders;
    std::unique_ptr<FileSystem> m_FS;
};