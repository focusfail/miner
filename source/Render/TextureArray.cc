#include "Render/TextureArray.hh"
#include "Assets/AssetsManager.hh"
#include "spdlog/spdlog.h"
#include <glad/gl.h>
#include <stb_image.h>

void TextureArray::Init() {
    int tSize = 32;
    int tLayers = 512;

    m_Width = tSize;
    m_Height = tSize;

    glCreateTextures(GL_TEXTURE_2D_ARRAY, 1, &m_Texture);
    glTextureStorage3D(m_Texture, 4, GL_RGBA8, tSize, tSize, tLayers);
    glTextureParameteri(m_Texture, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(m_Texture, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTextureParameteri(m_Texture, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(m_Texture, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

void TextureArray::Bind() { glBindTexture(GL_TEXTURE_3D, m_Texture); }

int TextureArray::AddTexture(const std::string &name) {
    auto &assets = AssetsManager::GetInstance();
    auto path = "/textures/" + name + ".png";
    auto buffer = assets.GetData(path);
    if (buffer.empty()) {
        spdlog::error("[TextureArray] Failed to find texture: {}", path);
        exit(1);
    }

    int w, h, c;
    auto data = stbi_load_from_memory(buffer.data(), buffer.size(), &w, &h, &c,
                                      STBI_rgb_alpha);

    if (m_Width < w || m_Height < h) {
        spdlog::warn("[TextureArray] Texture too big to add to array: {}",
                     name);
        return -1;
    }

    glTextureSubImage3D(m_Texture, 0, 0, 0, 0, m_Width, m_Height, m_NumLayers,
                        GL_RGBA, GL_UNSIGNED_BYTE, data);
    stbi_image_free(data);

    return m_NumLayers++;
}