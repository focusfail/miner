#pragma once

#include <GLFW/glfw3.h>
#include <array>
#include <functional>
#include <glm/glm.hpp>
#include <magic_enum/magic_enum.hpp>
#include <vector>

#include "Platform/Keys.hh"

enum class InputTrigger {
    PRESS,     // triggers on key down
    RELEASE,   // triggers on key up
    CONTINUOS, // triggers as long as key down
    HELD,      // triggers after a time holding
    DOUBLE,    // triggers after pressing twice
};

struct Keybind {
    KeyboardKey mod = KeyboardKey::NONE;
    KeyboardKey key = KeyboardKey::NONE;
    MouseButton btn = MouseButton::NONE;
    InputTrigger trigger = InputTrigger::PRESS;
    float timeout = 0.0f;
    float timer = 0.0f;
    float doubleTimer = 0.0f;
    int count = 0;
    std::function<void()> cb;
};

class Input {
  public:
    Input() = default;
    ~Input() = default;

    void Init(GLFWwindow *windowPtr);
    void Begin(float dt);
    void End();

    auto KeyPressed(KeyboardKey k) const -> bool;
    auto KeyReleased(KeyboardKey k) const -> bool;
    auto KeyDown(KeyboardKey k) const -> bool;

    auto MouseButtonPressed(MouseButton b) const -> bool;
    auto MouseButtonReleased(MouseButton b) const -> bool;
    auto MouseButtonDown(MouseButton b) const -> bool;

    auto GetMousePosition() const -> glm::vec2;
    auto GetMouseDelta() const -> glm::vec2;

    void RegisterKeybind(Keybind kb);

    Input &operator=(const Input &) = delete;
    Input(const Input &) = delete;

  private:
    static void KeyFun(GLFWwindow *, int, int, int, int);
    static void MouseBtnFun(GLFWwindow *, int, int, int);
    static void MouseFun(GLFWwindow *, double, double);

    // note: enums have extra NONE value at end which is initialized to false
    // and never touched
    std::array<bool, NUM_MOUSE_BUTTONS> m_OldMouse;
    std::array<bool, NUM_MOUSE_BUTTONS> m_NewMouse;
    std::array<bool, NUM_KEYBOARD_KEYS> m_OldKeys;
    std::array<bool, NUM_KEYBOARD_KEYS> m_NewKeys;
    glm::vec2 m_OldMousePosition;
    glm::vec2 m_NewMousePosition;

    std::vector<Keybind> m_Keybinds;
};
