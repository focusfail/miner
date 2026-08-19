#pragma once

#include "glm/glm.hpp"
#include <cstdint>
#include <source_location>
#include <string>

#include "Util/NonCopyable.hh"
#include "Util/NonMoveable.hh"

class Shader : private NonMovable, private NonCopyable {
  public:
    Shader() = default;
    ~Shader();

    void Load(const std::string &v, const std::string &f,
              std::source_location loc = std::source_location::current());
    void LoadStr(const std::string &v, const std::string &f,
                 std::source_location loc = std::source_location::current());
    void Unload();
    auto IsValid() const -> bool { return m_Program != 0; }
    void Use();

    void SetUniform(uint32_t location, const glm::mat4 &matrix);
    void SetUniform(uint32_t location, const glm::vec3 &vector);

  private:
    static uint32_t
    CompileShader(const std::string &source, uint32_t type,
                  std::source_location loc = std::source_location::current());

  private:
    uint32_t m_Program = 0;
};