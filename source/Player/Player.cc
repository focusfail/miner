#include "Player/Player.hh"
#include "Platform/Keys.hh"
#include <cmath>

Player::Player() {
    float height = 1.9f;
    float eyeHeight = 1.8f;

    m_Position = {0, 0, 0};
    m_Collider = Capsule(m_Position, height, 0.3f);
    m_Camera.Init(m_Position + glm::vec3(0, eyeHeight, 0));
}
static constexpr float EPSILON = 0.001f;

float ResolveAxisMovement(glm::vec3 &position, Capsule &capsule, float delta,
                          int axis, World &world) {
    if (std::abs(delta) < 0.0001f)
        return 0.0f;

    const float direction = delta;
    position[axis] += delta;

    constexpr int MAX_ITERATIONS = 4;
    for (int iter = 0; iter < MAX_ITERATIONS; ++iter) {
        capsule.position = position;
        glm::vec3 minBounds, maxBounds;
        capsule.GetAABB(minBounds, maxBounds);

        int minX = static_cast<int>(std::floor(minBounds.x - EPSILON));
        int maxX = static_cast<int>(std::floor(maxBounds.x + EPSILON));
        int minY = static_cast<int>(std::floor(minBounds.y - EPSILON));
        int maxY = static_cast<int>(std::floor(maxBounds.y + EPSILON));
        int minZ = static_cast<int>(std::floor(minBounds.z - EPSILON));
        int maxZ = static_cast<int>(std::floor(maxBounds.z + EPSILON));

        bool foundOverlap = false;
        float worstOverlap = -1.0f;
        float correction = 0.0f;

        for (int x = minX; x <= maxX; ++x) {
            for (int y = minY; y <= maxY; ++y) {
                for (int z = minZ; z <= maxZ; ++z) {
                    if (!world.IsBlockSolidAt({x, y, z}))
                        continue;

                    float blockMin =
                        static_cast<float>(axis == 0 ? x : (axis == 1 ? y : z));
                    float blockMax = blockMin + 1.0f;

                    bool overlapsX = maxBounds.x > x && minBounds.x < x + 1.0f;

                    bool overlapsY = maxBounds.y > y && minBounds.y < y + 1.0f;

                    bool overlapsZ = maxBounds.z > z && minBounds.z < z + 1.0f;

                    if (!overlapsX || !overlapsY || !overlapsZ)
                        continue;

                    float capMin = minBounds[axis];
                    float capMax = maxBounds[axis];

                    if (capMax > blockMin && capMin < blockMax) {
                        float overlap = (direction > 0.0f)
                                            ? (capMax - blockMin)
                                            : (capMin - blockMax); // negative
                        float depth = std::abs(overlap);
                        if (depth > worstOverlap) {
                            worstOverlap = depth;
                            correction = (direction > 0.0f) ? -(depth + EPSILON)
                                                            : (depth + EPSILON);
                            foundOverlap = true;
                        }
                    }
                }
            }
        }

        if (!foundOverlap) {
            capsule.position = position;
            return delta;
        }

        position[axis] += correction;
        delta = 0.0f;
    }

    capsule.position = position;
    return 0.0f;
}

void Player::Update(float dt, World &world, const Input &input) {
    glm::vec3 inputDir(0.0f);
    if (input.KeyDown(KeyboardKey::W))
        inputDir.z += 1.0f;
    if (input.KeyDown(KeyboardKey::S))
        inputDir.z -= 1.0f;
    if (input.KeyDown(KeyboardKey::A))
        inputDir.x -= 1.0f;
    if (input.KeyDown(KeyboardKey::D))
        inputDir.x += 1.0f;
    if (input.KeyDown(KeyboardKey::SPACE) && m_Flight)
        inputDir.y += 1.0f;
    if (input.KeyDown(KeyboardKey::LEFT_CONTROL) && m_Flight)
        inputDir.y -= 1.0f;

    glm::vec3 forward = m_Camera.GetLookDirection();
    forward.y = 0.0f;

    if (glm::length(forward) > 0.0001f) {
        forward = glm::normalize(forward);

    } else {
        forward = glm::vec3(0.0f, 0.0f, -1.0f);
    }

    glm::vec3 right = glm::cross(forward, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::vec3 vel = (forward * inputDir.z) + (right * inputDir.x);
    vel.y = inputDir.y;

    if (glm::length(vel) > 0.0001f) {
        vel = glm::normalize(vel);
    }

    float moveSpeed = 5.0f;
    glm::vec3 moveDelta = vel * moveSpeed * dt;

    constexpr float JUMP_IMPULSE = 8.0f;
    constexpr float GRAVITY = -25.0f;

    if (input.KeyDown(KeyboardKey::SPACE) && m_Grounded && !m_Flight) {
        m_Velocity.y = JUMP_IMPULSE;
        m_Grounded = false;
    }

    if (!m_Flight) {
        m_Velocity.y += GRAVITY * dt;
        moveDelta.y = m_Velocity.y * dt;
    }

    float intendedY = moveDelta.y;
    float resolvedY =
        ResolveAxisMovement(m_Position, m_Collider, moveDelta.y, 1, world);

    if (intendedY < 0.0f && std::abs(resolvedY) < 0.0001f) {
        m_Grounded = true;
        m_Velocity.y = 0.0f;
    } else {
        m_Grounded = false;
    }

    if (intendedY > 0.0f && std::abs(resolvedY) < 0.0001f) {
        m_Velocity.y = 0.0f;
    }

    ResolveAxisMovement(m_Position, m_Collider, moveDelta.x, 0, world);
    ResolveAxisMovement(m_Position, m_Collider, moveDelta.z, 2, world);

    m_Collider.position = m_Position;
    m_Camera.SetPosition(m_Position + glm::vec3(0.0f, 1.8f, 0.0f));
}

void Player::SetPosition(const glm::vec3 &pos) {
    m_Position = pos;
    m_Collider.position = pos;
    m_Camera.SetPosition(m_Position + glm::vec3(0.0f, 1.8f, 0.0f));
}