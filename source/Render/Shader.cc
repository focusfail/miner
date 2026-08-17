#include "Render/Shader.hh"
#include "Util/File.hh"

#include <glad/gl.h>
#include <glm/gtc/type_ptr.hpp>
#include <spdlog/spdlog.h>

Shader::~Shader() {
  if (m_Program != 0) {
    glDeleteProgram(m_Program);
  }
}

void Shader::Load(const std::string &vFile, const std::string &fFile) {
  std::string v = ReadFile(vFile);
  std::string f = ReadFile(fFile);

  uint32_t vertexShader = CompileShader(v, GL_VERTEX_SHADER);
  uint32_t fragmentShader = CompileShader(f, GL_FRAGMENT_SHADER);
  uint32_t program = glCreateProgram();
  glAttachShader(program, vertexShader);
  glAttachShader(program, fragmentShader);
  glLinkProgram(program);

  GLint ok;
  glGetProgramiv(program, GL_LINK_STATUS, &ok);
  if (ok == GL_FALSE) {
    int len;
    char log[1024];
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);
    glGetProgramInfoLog(program, 1024, &len, log);
    spdlog::warn("Failed to link shader program: {}", log);
  }

  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  m_Program = program;
  m_FragmentShaderName = f;
  m_VertexShaderName = v;
}

void Shader::Use() { glUseProgram(m_Program); }

void Shader::SetUniform(uint32_t location, const glm::mat4 &matrix) {
  glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

void Shader::SetUniform(uint32_t location, const glm::vec3 &vector) {
  glUniform3fv(location, 1, glm::value_ptr(vector));
}

uint32_t Shader::CompileShader(const std::string &source, uint32_t type) {
  uint32_t shader = glCreateShader(type);
  const char *cSource = source.c_str();
  int sourceLen = source.size();
  glShaderSource(shader, 1, &cSource, &sourceLen);
  glCompileShader(shader);

  GLint ok;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);

  if (ok == GL_FALSE) {
    GLint len = 0;
    char log[1024];
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
    glGetShaderInfoLog(shader, 1024, &len, &log[0]);
    std::string typeStr = type == GL_VERTEX_SHADER ? "vertex" : "fragment";
    spdlog::warn("Failed to compile {} shader: {}", typeStr, log);
  }

  return shader;
}