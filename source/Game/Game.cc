#include "Game/Game.hh"
#include "Assets/AssetsManager.hh"
#include "Platform/Input.hh"
#include "Platform/Keys.hh"
#include "Render/Camera.hh"
#include "TaskScheduler.h"
#include "World/Coordinates.hh"
#include "World/World.hh"
#include <glad/gl.h>
#include <memory>
#include <spdlog/spdlog.h>

void Game::Init() {
    spdlog::info("[Game] Init");
    m_Window.Init("mine", 1920, 1080);
    m_Window.CaptureMouse(true);

    gladLoaderLoadGL();
    glViewport(0, 0, m_Window.GetWidth(), m_Window.GetHeight());
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glLineWidth(5.0f);

    AssetsManager::GetInstance().Init();
    m_Sched.Initialize();
    m_DebugRenderer.Init();
    m_BlockOverlayRenderer.Init();
    m_World.Init();

    m_Player.SetPosition({8.5, 2, 8.5});
}

void Game::Mainloop() {
    auto &input = m_Window.GetInput();

    Keybind toggleWireframe{
        .key = KeyboardKey::F4, .cb = [this]() {
            auto &s = GetDebugState();
            glPolygonMode(s.drawWireframe ? GL_FRONT : GL_FRONT_AND_BACK,
                          s.drawWireframe ? GL_FILL : GL_LINE);
            s.drawWireframe = !s.drawWireframe;
        }};
    Keybind toggleDebugDraw{.key = KeyboardKey::F3, .cb = [this]() {
                                auto &s = GetDebugState();
                                s.debugDraw = !s.debugDraw;
                            }};

    Keybind mineBlock{
        .btn = MouseButton::LMB,
        .trigger = InputTrigger::CONTINUOS,
        .cb = [this]() {
            glm::vec3 start = m_Player.GetEyePosition();
            glm::vec3 end = start + m_Player.GetLookDirection() * 10.0f;
            if (auto hit = m_World.CastRay(start, end)) {
                if (m_GameState.blockMineTimer < m_GameState.blockMineTimeout) {
                    m_GameState.blockMineTimer += m_Window.GetDeltaTime();
                    return;
                }
                m_GameState.blockMineTimer = 0.0f;

                if (auto chunk =
                        m_World.TryGetChunkDataByPosition(hit->chunkPosition)) {
                    chunk->SetBlockBreakStage(
                        hit->blockPosition,
                        chunk->GetBlockBreakStage(hit->blockPosition) + 1);
                }
                m_DebugRenderer.DrawLine(start, hit->position, 1.0f);
            }
        }};

    Keybind doubleTest{
        .key = KeyboardKey::SPACE,
        .trigger = InputTrigger::DOUBLE,
        .cb = [this]() { m_Player.SetFlight(!m_Player.GetFlight()); }};

    input.RegisterKeybind(toggleWireframe);
    input.RegisterKeybind(toggleDebugDraw);
    input.RegisterKeybind(doubleTest);
    input.RegisterKeybind(mineBlock);

    while (!m_Window.ShouldClose()) {
        m_Window.Begin();

        glm::vec3 start = m_Player.GetEyePosition();
        glm::vec3 end = start + m_Player.GetLookDirection() * 10.0f;
        if (auto hit = m_World.CastRay(start, end)) {
            if (hit->blockPosition.IsValid()) {
                glm::ivec3 worldPos =
                    (hit->chunkPosition * static_cast<int>(Chunk::Size)) +
                    glm::ivec3(hit->blockPosition);
                float alpha = 0.2f + (0.8f / 16) * hit->block.breakStage;
                m_BlockOverlayRenderer.DrawBox(worldPos,
                                               {0.0f, 0.0f, 0.0f, alpha});
            }
        }

        if (m_Window.IsResized()) {
            glViewport(0, 0, m_Window.GetWidth(), m_Window.GetHeight());
        }
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float dt = m_Window.GetDeltaTime();
        auto &camera = m_Player.GetCamera();
        m_Player.Update(dt, m_World, input);
        camera.ProcessMouse(input.GetMouseDelta());

        m_World.Update();
        m_World.Render(camera);

        if (m_DebugState.debugDraw) {
            m_DebugRenderer.Render(camera, dt);
        }
        m_BlockOverlayRenderer.Render(camera);

        m_Window.End();
    }
}
