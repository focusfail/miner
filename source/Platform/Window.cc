#include "Platform/Window.hh"
#include "GLFW/glfw3.h"
#include "Platform/Keys.hh"

#include <spdlog/spdlog.h>

Window::Window(std::string title, int width, int height) {
    if (!glfwInit()) {
        return;
    }

    m_Glfw = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    
    if (!m_Glfw) {
        return;
    }

    float x, y;
    
    glfwMakeContextCurrent(m_Glfw);
    glfwSetWindowUserPointer(m_Glfw, this);
    
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
}

std::shared_ptr<Window> Window::Create(std::string title, int width,
                                       int height) {
    auto window =
        std::shared_ptr<Window>(new Window(std::move(title), width, height));
    return window;
}

bool Window::ShouldClose() const {
    return glfwWindowShouldClose(m_Glfw) ||
           m_Input.KeyDown(KeyboardKey::ESCAPE);
}

void Window::Begin() {
    glfwPollEvents();
    m_NewTime = glfwGetTime();
    m_Input.Begin(GetDeltaTime());
}

void Window::End() {
    glfwSwapBuffers(m_Glfw);
    m_OldTime = m_NewTime;
    m_Input.End();
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

float Window::GetDeltaTime() const {
    return static_cast<float>(m_NewTime - m_OldTime);
}

void Window::CaptureMouse(bool shouldCapture) {
    if (shouldCapture) {
        glfwSetInputMode(m_Glfw, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    } else {
        glfwSetInputMode(m_Glfw, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
}

Input &Window::GetInput() { return m_Input; }
GLFWwindow *Window::GetGlfwWindow() const { return m_Glfw; }