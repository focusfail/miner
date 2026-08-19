#include "Render/ShaderProgram.hh"
#include "Util/File.hh"

#include <glad/gl.h>
#include <glm/gtc/type_ptr.hpp>
#include <source_location>
#include <spdlog/spdlog.h>

ShaderProgram::~ShaderProgram() { Unload(); }

void ShaderProgram::Load(const std::string &vFile, const std::string &fFile,
                         std::source_location loc) {
    std::string v = ReadFile(vFile);
    std::string f = ReadFile(fFile);
    LoadStr(v, f, loc);
}

void ShaderProgram::Unload() {
    if (m_Program != 0) {
        glDeleteProgram(m_Program);
    }
}

void ShaderProgram::LoadStr(const std::string &v, const std::string &f,
                            std::source_location loc) {
    uint32_t vertexShader = CompileShader(v, GL_VERTEX_SHADER, loc);
    uint32_t fragmentShader = CompileShader(f, GL_FRAGMENT_SHADER, loc);
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
        spdlog::warn("[{}:{}] Failed to link shader program: {}",
                     loc.file_name(), loc.line(), log);
        m_Program = 0;
        return;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    m_Program = program;
}

void ShaderProgram::Bind() { glUseProgram(m_Program); }

void ShaderProgram::SetUniform(uint32_t location, const glm::mat4 &matrix) {
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

void ShaderProgram::SetUniform(uint32_t location, const glm::vec3 &vector) {
    glUniform3fv(location, 1, glm::value_ptr(vector));
}

uint32_t ShaderProgram::CompileShader(const std::string &source, uint32_t type,
                                      std::source_location loc) {
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
        spdlog::warn("[{}:{}] Failed to compile {} shader: {}", loc.file_name(),
                     loc.line(), typeStr, log);
        return 0;
    }

    return shader;
}