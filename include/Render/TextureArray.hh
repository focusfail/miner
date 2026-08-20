#pragma once
#include "Util/NonCopyable.hh"
#include "Util/NonMoveable.hh"
#include <cstdint>

#include <string>

class TextureArray : private NonCopyable, private NonMovable {
  public:
    void Init();
    void Bind();

    int AddTexture(const std::string &name);

    int GetHeight() const { return m_Height; }
    int GetWidth() const { return m_Width; }
    int GetNumLayers() const { return m_NumLayers; }
    int GetMaxLayers() const { return m_MaxLayers; }

  private:
    uint32_t m_Texture = 0;
    int m_Width = 0;
    int m_Height = 0;
    int m_MaxLayers = 0;
    int m_NumLayers = 0;
};