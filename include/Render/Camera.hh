#pragma once

#include "World/Coordinates.hh"

#include <bitset>
#include <glm/glm.hpp>

class Camera {
  public:
    Camera() = default;
    ~Camera() = default;

    void Init(glm::vec3 position = {0, 0, 0});
    void ProcessMouse(const glm::vec2 delta);
    void ProcessMovement(bool fwd, bool bwd, bool left, bool right, bool up,
                         bool down, float velocity);

    void SetPosition(glm::vec3 position);

    glm::vec3 GetWorldUp() const { return m_WorldUp; }
    glm::vec3 GetRight() const { return m_Right; }
    glm::vec3 GetPosition() const { return m_Position; }
    glm::vec3 GetLookDirection() const { return m_Front; }

    glm::mat4 GetViewMatrix() const;

  private:
    void UpdateVectors();

    glm::vec3 m_Position;
    glm::vec3 m_Front;
    glm::vec3 m_Right;
    glm::vec3 m_Up;
    glm::vec3 m_WorldUp;
    float m_Yaw;
    float m_Pitch;
    float m_Zoom;
};