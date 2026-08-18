#pragma once

#include "glm/glm.hpp"
#include <cstdint>
#include <string>

class Shader {
  public:
    Shader() = default;
    ~Shader();

    void Load(const std::string &v, const std::string &f);
    void LoadStr(const std::string &v, const std::string &f);
    void Unload();
    void Use();

    void SetUniform(uint32_t location, const glm::mat4 &matrix);
    void SetUniform(uint32_t location, const glm::vec3 &vector);

  private:
    static uint32_t CompileShader(const std::string &source, uint32_t type);

  private:
    uint32_t m_Program = 0;
    std::string m_VertexShaderName;
    std::string m_FragmentShaderName;
};