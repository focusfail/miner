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

    Shader *GetShader(const std::string &name);

  private:
    AssetsManager();
    ~AssetsManager();
    struct FileSystem;

    std::unordered_map<std::string, std::unique_ptr<Shader>> m_Shaders;
    std::unique_ptr<FileSystem> m_FS;
};