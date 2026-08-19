#pragma once

#include <Render/ShaderProgram.hh>
#include <Render/Texture.hh>
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

    ShaderProgram *GetShaderProgram(const std::string &name);
    Texture *GetTexture(const std::string &name);

  private:
    AssetsManager();
    ~AssetsManager();
    struct FileSystem;

    std::unordered_map<std::string, std::unique_ptr<ShaderProgram>> m_Shaders;
    std::unordered_map<std::string, std::unique_ptr<Texture>> m_Textures;
    std::unique_ptr<FileSystem> m_FS;
};