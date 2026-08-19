#pragma once

#include "Util/NonCopyable.hh"
#include "Util/NonMoveable.hh"
#include <cstdint>

#include <vector>

class Texture : private NonMovable, private NonCopyable {
  public:
    void Load(std::vector<uint8_t> buffer);
    bool IsValid() const { return m_Texture != 0; }

    int GetWidth() const { return m_Width; }
    int GetHeight() const { return m_Height; }

  private:
    int m_Width = 0;
    int m_Height = 0;
    uint32_t m_Texture = 0;
};