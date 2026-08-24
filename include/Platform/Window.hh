#pragma once

#include "Platform/Input.hh"
#include "Util/NonCopyable.hh"
#include "Util/NonMoveable.hh"

#include <memory>
#include <string>

#include <GLFW/glfw3.h>

class Window : std::enable_shared_from_this<Window>, private NonCopyable, private NonMovable {
public:
    Window() = default;
    ~Window();

    void Init(std::string title, int width, int height);
    void Begin();
    void End();
    bool ShouldClose() const;

    void FitToScreen();

    int GetWidth() const;
    int GetHeight() const;
    float GetDeltaTime() const;

    bool IsResized() const { return m_IsResized; }

    void Resize(int w, int h);
    void CaptureMouse(bool shouldCapture);
    bool IsMouseCaptured() const { return m_IsMouseCaptured; }

    Input &GetInput();
    GLFWwindow *GetGlfwWindow() const;

private:
    static void ResizeFun(GLFWwindow *glfw, int newX, int newY);

    bool m_IsResized = false;
    bool m_IsMouseCaptured = false;

    double m_OldTime;
    double m_NewTime;
    GLFWwindow *m_Glfw;
    Input m_Input;
};