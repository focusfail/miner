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

void Input::Update() {
  std::copy(m_NewKeys.begin(), m_NewKeys.end(), m_OldKeys.begin());
  m_OldMousePosition = m_NewMousePosition;
}

bool Input::KeyDown(KeyboardKey k) const {
  return m_NewKeys[static_cast<size_t>(k)];
}

bool Input::KeyPressed(KeyboardKey k) const {
  return m_NewKeys[static_cast<size_t>(k)] &&
         !m_OldKeys[static_cast<size_t>(k)];
}

bool Input::KeyReleased(KeyboardKey k) const {
  return !m_NewKeys[static_cast<size_t>(k)] &&
         m_OldKeys[static_cast<size_t>(k)];
}

glm::vec2 Input::GetMousePosition() const { return m_NewMousePosition; }
glm::vec2 Input::GetMouseDelta() const {
  return m_NewMousePosition - m_OldMousePosition;
}