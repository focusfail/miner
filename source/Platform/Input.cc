#include "Platform/Input.hh"
#include "Platform/Keys.hh"
#include "Platform/Window.hh"

#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>

void Input::Init(GLFWwindow *windowPtr) {
    glfwSetKeyCallback(windowPtr, Input::KeyFun);
    glfwSetCursorPosCallback(windowPtr, Input::MouseFun);
    glfwSetMouseButtonCallback(windowPtr, Input::MouseBtnFun);
    m_NewKeys.fill(false);
    m_OldKeys.fill(false);
    m_NewMouse.fill(false);
    m_OldMouse.fill(false);
    spdlog::info("{} {}", NUM_KEYBOARD_KEYS, NUM_MOUSE_BUTTONS);
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

void Input::MouseBtnFun(GLFWwindow *windowPtr, int btn, int ac, int mods) {
    auto window = static_cast<Window *>(glfwGetWindowUserPointer(windowPtr));
    auto &input = window->GetInput();

    switch (ac) {
    case GLFW_PRESS:
        input.m_NewMouse[btn] = true;
        break;
    case GLFW_RELEASE:
        input.m_NewMouse[btn] = false;
        break;
    }
}
void Input::Begin(float dt) {
    for (auto &bind : m_Keybinds) {
        if (bind.doubleTimer > 0.0f) {
            bind.doubleTimer = fmax(bind.doubleTimer - dt, 0.0f);
            if (bind.doubleTimer == 0.0f) {
                bind.count = 0;
            }
        }

        if (bind.timeout > 0.0f && bind.timer < bind.timeout) {
            bind.timer += dt;
            continue;
        }

        bool mod = bind.mod == KeyboardKey::NONE || KeyDown(bind.mod);
        bool hasKey = bind.key != KeyboardKey::NONE;
        bool hasBtn = bind.btn != MouseButton::NONE;

        switch (bind.trigger) {
        case InputTrigger::PRESS: {
            bool key = hasKey && KeyPressed(bind.key);
            bool btn = hasBtn && MouseButtonPressed(bind.btn);
            if (mod && (key || btn)) {
                bind.cb();
                bind.timer = 0.0f;
            }
        } break;
        case InputTrigger::RELEASE: {
            bool key = hasKey && KeyReleased(bind.key);
            bool btn = hasBtn && MouseButtonReleased(bind.btn);
            if (mod && (key || btn)) {
                bind.cb();
                bind.timer = 0.0f;
            }
        } break;
        case InputTrigger::CONTINUOS: {
            bool key = hasKey && KeyDown(bind.key);
            bool btn = hasBtn && MouseButtonDown(bind.btn);
            if (mod && (key || btn)) {
                bind.cb();
                bind.timer = 0.0f;
            }
        } break;
        case InputTrigger::DOUBLE: {
            bool key = hasKey && KeyPressed(bind.key);
            bool btn = hasBtn && MouseButtonPressed(bind.btn);

            if (mod && (key || btn)) {
                if (bind.count == 0) {
                    bind.count = 1;
                    bind.doubleTimer = 0.2f;
                } else if (bind.count == 1 && bind.doubleTimer > 0.0f) {
                    bind.cb();
                    bind.count = 0;
                    bind.doubleTimer = 0.0f;
                    bind.timer = 0.0f;
                }
            }
        } break;

        default:
            break;
        }
    }
}

void Input::End() {
    std::copy(m_NewKeys.begin(), m_NewKeys.end(), m_OldKeys.begin());
    std::copy(m_NewMouse.begin(), m_NewMouse.end(), m_OldMouse.begin());
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

auto Input::MouseButtonDown(MouseButton b) const -> bool {
    return m_NewMouse[static_cast<size_t>(b)];
}

auto Input::MouseButtonPressed(MouseButton b) const -> bool {
    return m_NewMouse[static_cast<size_t>(b)] &&
           !m_OldMouse[static_cast<size_t>(b)];
}

auto Input::MouseButtonReleased(MouseButton b) const -> bool {
    return !m_NewMouse[static_cast<size_t>(b)] &&
           m_OldMouse[static_cast<size_t>(b)];
}

void Input::RegisterKeybind(Keybind kb) {
    kb.timer = kb.timeout; // keybind is available immediatly
    m_Keybinds.emplace_back(kb);
}

auto Input::GetMousePosition() const -> glm::vec2 { return m_NewMousePosition; }
auto Input::GetMouseDelta() const -> glm::vec2 {
    return m_NewMousePosition - m_OldMousePosition;
}