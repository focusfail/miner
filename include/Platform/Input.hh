#pragma once

#include <GLFW/glfw3.h>
#include <array>
#include <glm/glm.hpp>

#include "Platform/Keys.hh"

class Input {
public:
  Input() = default;
  ~Input() = default;

  void Init(GLFWwindow *windowPtr);
  void Update();
  bool KeyPressed(KeyboardKey k) const;
  bool KeyReleased(KeyboardKey k) const;
  bool KeyDown(KeyboardKey k) const;

  glm::vec2 GetMousePosition() const;
  glm::vec2 GetMouseDelta() const;

  Input &operator=(const Input &) = delete;
  Input(const Input &) = delete;

private:
  static void KeyFun(GLFWwindow *, int, int, int, int);
  static void MouseFun(GLFWwindow *, double, double);

  std::array<bool, KEYBOARD_KEY_LAST> m_OldKeys;
  std::array<bool, KEYBOARD_KEY_LAST> m_NewKeys;
  glm::vec2 m_OldMousePosition;
  glm::vec2 m_NewMousePosition;
};