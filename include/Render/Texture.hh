#pragma once

#include "Util/NonCopyable.hh"
#include "Util/NonMoveable.hh"
#include <cstdint>

class Texture : private NonMovable, private NonCopyable {

    int GetWidth() const { return m_Width; }
    int GetHeight() const { return m_Height; }

  private:
    int m_Width = 0;
    int m_Height = 0;
    uint32_t m_TextureHandle = 0;
};