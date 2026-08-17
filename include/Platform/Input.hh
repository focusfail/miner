#pragma once

#include <GLFW/glfw3.h>
#include <array>
#include <functional>
#include <glm/glm.hpp>
#include <vector>

#include "Platform/Keys.hh"

enum class KeyTrigger {
  PRESSED,
  RELEASE,
  HELD,
};

struct Keybind {
  KeyboardKey key;
  KeyTrigger trigger;
  std::function<void()> cb;
};
class Input {
public:
  Input() = default;
  ~Input() = default;

  void Init(GLFWwindow *windowPtr);
  void Begin();
  void End();

  auto KeyPressed(KeyboardKey k) const -> bool;
  auto KeyReleased(KeyboardKey k) const -> bool;
  auto KeyDown(KeyboardKey k) const -> bool;

  auto GetMousePosition() const -> glm::vec2;
  auto GetMouseDelta() const -> glm::vec2;

  void RegisterKeybind(KeyboardKey key, std::function<void()> callback,
                       KeyTrigger trigger = KeyTrigger::PRESSED);

  Input &operator=(const Input &) = delete;
  Input(const Input &) = delete;

private:
  static void KeyFun(GLFWwindow *, int, int, int, int);
  static void MouseFun(GLFWwindow *, double, double);

  std::array<bool, KEYBOARD_KEY_LAST> m_OldKeys;
  std::array<bool, KEYBOARD_KEY_LAST> m_NewKeys;
  glm::vec2 m_OldMousePosition;
  glm::vec2 m_NewMousePosition;

  std::vector<Keybind> m_Keybinds;
};