#pragma once

#include "Platform/Input.hh"
#include "Util/NonCopyable.hh"
#include "Util/NonMoveable.hh"

#include <memory>
#include <string>

#include <GLFW/glfw3.h>

class Window : std::enable_shared_from_this<Window>,
               private NonCopyable,
               private NonMovable {
public:
  ~Window();

  static std::shared_ptr<Window> Create(std::string title, int width,
                                        int height);
  void Begin();
  void End();
  bool ShouldClose() const;

  int GetWidth() const;
  int GetHeight() const;

  void CaptureMouse(bool shouldCapture);

  Input &GetInput();
  GLFWwindow *GetGlfwWindow() const;

private:
  Window(std::string title, int width, int height);

  GLFWwindow *m_Glfw;
  Input m_Input;
};