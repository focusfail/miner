#include "Render/Texture.hh"
#include "spdlog/spdlog.h"
#include <glad/gl.h>
// #define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

void Texture::Load(std::vector<uint8_t> buffer) {
    int w, h, c;
    auto data = stbi_load_from_memory(buffer.data(), buffer.size(), &w, &h, &c,
                                      STBI_rgb_alpha);

    if (!data) {
        spdlog::error("[Texture] Failed to load image");
        exit(1);
    }

    glCreateTextures(GL_TEXTURE_2D, 1, &m_Texture);
    glTextureParameteri(m_Texture, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(m_Texture, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTextureParameteri(m_Texture, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(m_Texture, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTextureStorage2D(m_Texture, 1, GL_RGBA8, w, h);
    glTextureSubImage2D(m_Texture, 0, 0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE,
                        data);
    stbi_image_free(data);
}

void Texture::Bind() { glBindTexture(GL_TEXTURE_2D, m_Texture); }