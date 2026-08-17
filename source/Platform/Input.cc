#include "Platform/Input.hh"
#include "Platform/Keys.hh"
#include "Platform/Window.hh"

#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>

void Input::Init(GLFWwindow *windowPtr) {
  glfwSetKeyCallback(windowPtr, Input::KeyFun);
  glfwSetCursorPosCallback(windowPtr, Input::MouseFun);
  m_NewKeys.fill(false);
  m_OldKeys.fill(false);
}

void Input::KeyFun(GLFWwindow *windowPtr, int key, int sc, int ac, int mods) {
  auto window = static_cast<Window *>(glfwGetWindowUserPointer(windowPtr));
  auto &input = window->GetInput();
  switch (ac) {
  case GLFW_PRESS: {
    input.m_NewKeys[key] = true;
  } break;
  case GLFW_RELEASE: {
    input.m_NewKeys[key] = false;
  }
  }
}
void Input::MouseFun(GLFWwindow *windowPtr, double x, double y) {
  auto window = static_cast<Window *>(glfwGetWindowUserPointer(windowPtr));
  auto &input = window->GetInput();
  input.m_NewMousePosition = {x, y};
}

void Input::Begin() {
  for (auto &bind : m_Keybinds) {
    switch (bind.trigger) {
    case KeyTrigger::PRESSED:
      if (KeyPressed(bind.key)) {
        bind.cb();
      }
      break;
    case KeyTrigger::RELEASE:
      if (KeyReleased(bind.key))
        bind.cb();
      break;
    case KeyTrigger::HELD:
      if (KeyDown(bind.key))
        bind.cb();
      break;
    }
  }
}

void Input::End() {
  std::copy(m_NewKeys.begin(), m_NewKeys.end(), m_OldKeys.begin());
  m_OldMousePosition = m_NewMousePosition;
}

auto Input::KeyDown(KeyboardKey k) const -> bool {
  return m_NewKeys[static_cast<size_t>(k)];
}

auto Input::KeyPressed(KeyboardKey k) const -> bool {
  return m_NewKeys[static_cast<size_t>(k)] &&
         !m_OldKeys[static_cast<size_t>(k)];
}

auto Input::KeyReleased(KeyboardKey k) const -> bool {
  return !m_NewKeys[static_cast<size_t>(k)] &&
         m_OldKeys[static_cast<size_t>(k)];
}

void Input::RegisterKeybind(KeyboardKey k, std::function<void()> cb,
                            KeyTrigger t) {
  m_Keybinds.emplace_back(k, t, cb);
}

auto Input::GetMousePosition() const -> glm::vec2 { return m_NewMousePosition; }
auto Input::GetMouseDelta() const -> glm::vec2 {
  return m_NewMousePosition - m_OldMousePosition;
}