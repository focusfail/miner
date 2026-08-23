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
    bool GetFlight() const { return m_Flight; }
    void GetAABB(glm::vec3 &minBounds, glm::vec3 &maxBounds) const {
        return m_Collider.GetAABB(minBounds, maxBounds);
    }

    Camera &GetCamera() { return m_Camera; }
    glm::vec3 GetPosition() const { return m_Position; }
    glm::vec3 GetEyePosition() const { return m_Camera.GetPosition(); }
    glm::vec3 GetLookDirection() const { return m_Camera.GetLookDirection(); }

    glm::mat4 GetViewMatrix() const { return m_Camera.GetViewMatrix(); }

private:
    float MoveX(float dist, World &world);
    float MoveY(float dist, World &world);
    float MoveZ(float dist, World &world);

private:
    glm::vec3 m_Position{};
    glm::vec3 m_Velocity{};

    Camera m_Camera;
    Capsule m_Collider;

    bool m_Grounded = false;
    bool m_Flight = false;
};