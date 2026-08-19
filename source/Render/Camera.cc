#include "Render/Camera.hh"

#include "World/Coordinates.hh"
#include "glm/common.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/trigonometric.hpp"

void Camera::Init(glm::vec3 position) {
    m_Position = position;
    m_WorldUp = {0, 1, 0};
    m_Yaw = 90;
    m_Pitch = 0;
    m_Zoom = 45;
    UpdateVectors();
}

void Camera::SetPosition(glm::vec3 position) {
    m_Position = position;
    UpdateVectors();
}

void Camera::ProcessMouse(const glm::vec2 delta) {
    if (delta == glm::vec2(0, 0))
        return;

    const float sens = 0.1;
    m_Yaw += delta.x * sens;
    m_Pitch -= delta.y * sens;
    m_Pitch = glm::clamp(m_Pitch, -89.0f, 89.0f);
    UpdateVectors();
}

void Camera::ProcessMovement(bool fwd, bool bwd, bool left, bool right, bool up,
                             bool down, float velocity) {
    if (fwd) {
        m_Position += m_Front * velocity;
    }
    if (bwd) {
        m_Position -= m_Front * velocity;
    }
    if (left) {
        m_Position -= m_Right * velocity;
    }
    if (right) {
        m_Position += m_Right * velocity;
    }
    if (up) {
        m_Position += m_WorldUp * velocity;
    }
    if (down) {
        m_Position -= m_WorldUp * velocity;
    }
}

void Camera::UpdateVectors() {
    float yawRad = glm::radians(m_Yaw);
    float pitchRad = glm::radians(m_Pitch);

    glm::vec3 front;
    front.x = cos(yawRad) * cos(pitchRad);
    front.y = sin(pitchRad);
    front.z = sin(yawRad) * cos(pitchRad);
    m_Front = glm::normalize(front);
    m_Right = glm::normalize(glm::cross(m_Front, m_WorldUp));
    m_Up = glm::normalize(glm::cross(m_Right, m_Front));
}

glm::mat4 Camera::GetViewMatrix() const {
    return glm::lookAt(m_Position, m_Position + m_Front, m_Up);
}