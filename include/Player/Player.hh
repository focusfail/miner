#pragma once

#include "Platform/Input.hh"
#include "Player/Capsule.hh"
#include "Render/Camera.hh"
#include "Util/NonCopyable.hh"
#include "Util/NonMoveable.hh"
#include "World/World.hh"

class Player : private NonCopyable, private NonMovable {
  public:
    Player();

    void Update(float dt, World &world, const Input &input);
    void SetPosition(const glm::vec3 &pos);

    void SetFlight(bool enable) { m_Flight = enable; }
    auto GetFlight() const -> bool { return m_Flight; }

    auto GetCamera() -> Camera & { return m_Camera; }
    auto GetPosition() const -> glm::vec3 { return m_Position; }
    auto GetEyePosition() const -> glm::vec3 { return m_Camera.GetPosition(); }
    auto GetLookDirection() const -> glm::vec3 {
        return m_Camera.GetLookDirection();
    }

    auto GetViewMatrix() const -> glm::mat4 { return m_Camera.GetViewMatrix(); }

  private:
    auto MoveX(float dist, World &world) -> float;
    auto MoveY(float dist, World &world) -> float;
    auto MoveZ(float dist, World &world) -> float;

  private:
    glm::vec3 m_Position{};
    glm::vec3 m_Velocity{};

    Camera m_Camera;
    Capsule m_Collider;

    bool m_Grounded = false;
    bool m_Flight = false;
};