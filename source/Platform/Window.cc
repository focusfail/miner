#include "Platform/Window.hh"
#include "GLFW/glfw3.h"
#include "Platform/Keys.hh"

#include <spdlog/spdlog.h>

void Window::ResizeFun(GLFWwindow *glfw, int newX, int newY) {
    auto window = static_cast<Window *>(glfwGetWindowUserPointer(glfw));
    window->m_IsResized = true;
}

void Window::Init(std::string title, int width, int height) {
    if (!glfwInit()) {
        return;
    }

    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    m_Glfw = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);

    if (!m_Glfw) {
        return;
    }

    float x, y;

    glfwMakeContextCurrent(m_Glfw);
    glfwSetWindowUserPointer(m_Glfw, this);
    glfwSetWindowSizeCallback(m_Glfw, ResizeFun);

    auto mon = glfwGetPrimaryMonitor();
    glfwGetMonitorContentScale(mon, &x, &y);
    Resize(width / x, height / y);
    m_Input.Init(m_Glfw);
}

Window::~Window() {
    glfwDestroyWindow(m_Glfw);
    m_Glfw = nullptr;
    glfwTerminate();
}

void Window::FitToScreen() {
    auto mon = glfwGetPrimaryMonitor();
    int w, h;
    glfwGetMonitorPhysicalSize(mon, &w, &h);
    Resize(static_cast<int>(w * 0.9f), static_cast<int>(h * 0.9f));
}

void Window::Resize(int w, int h) {
    glfwSetWindowSize(m_Glfw, w, h);
    m_IsResized = true;
}

bool Window::ShouldClose() const { return glfwWindowShouldClose(m_Glfw) || m_Input.KeyDown(KeyboardKey::ESCAPE); }

void Window::Begin() {
    glfwPollEvents();
    m_NewTime = glfwGetTime();
    m_Input.Begin(GetDeltaTime());
}

void Window::End() {
    glfwSwapBuffers(m_Glfw);
    m_OldTime = m_NewTime;
    m_Input.End();
    m_IsResized = false;
}

int Window::GetWidth() const {
    int w, h;
    glfwGetFramebufferSize(m_Glfw, &w, &h);
    return w;
}

int Window::GetHeight() const {
    int w, h;
    glfwGetFramebufferSize(m_Glfw, &w, &h);
    return h;
}

float Window::GetDeltaTime() const { return static_cast<float>(m_NewTime - m_OldTime); }

void Window::CaptureMouse(bool shouldCapture) {
    if (shouldCapture) {
        glfwSetInputMode(m_Glfw, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        m_IsMouseCaptured = true;
    } else {
        glfwSetInputMode(m_Glfw, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        m_IsMouseCaptured = false;
    }
}

Input &Window::GetInput() { return m_Input; }
GLFWwindow *Window::GetGlfwWindow() const { return m_Glfw; }