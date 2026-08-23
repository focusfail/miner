#include "Game/Game.hh"
#include "Assets/AssetsManager.hh"
#include "Platform/Input.hh"
#include "Platform/Keys.hh"
#include "Render/Camera.hh"
#include "TaskScheduler.h"
#include "World/Block/BlockRegistry.hh"
#include "World/Coordinates.hh"
#include "World/World.hh"
#include <algorithm>
#include <glad/gl.h>
#include <optional>
#include <spdlog/spdlog.h>

#define NANOVG_GL3_IMPLEMENTATION
#include <nanovg.h>
#include <nanovg_gl.h>

NVGcontext *vg = nullptr;
BlockID currentBlock = 1;
BlockID maxBlock;

void Game::Init() {
    spdlog::info("[Game] Init");
    m_Window.Init("mine", 1920, 1080);
    m_Window.CaptureMouse(true);

    gladLoaderLoadGL();
    glViewport(0, 0, m_Window.GetWidth(), m_Window.GetHeight());
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glEnable(GL_STENCIL_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glLineWidth(5.0f);

    AssetsManager::GetInstance().Init();
    m_Sched.Initialize();
    m_DebugRenderer.Init();
    m_BlockOverlayRenderer.Init();
    m_World.Init();

    maxBlock = BlockRegistry::GetInstance().GetBlockCount();

    m_Player.SetPosition({8.5, 2, 8.5});

    vg = nvgCreateGL3(NVG_ANTIALIAS | NVG_STENCIL_STROKES | NVG_DEBUG);
}

void Game::Mainloop() {
    auto &input = m_Window.GetInput();

    Keybind toggleWireframe{.key = KeyboardKey::F4, .cb = [this]() {
        auto &s = GetDebugState();
        glPolygonMode(s.drawWireframe ? GL_FRONT : GL_FRONT_AND_BACK, s.drawWireframe ? GL_FILL : GL_LINE);
        s.drawWireframe = !s.drawWireframe;
    }};
    Keybind toggleDebugDraw{.key = KeyboardKey::F3, .cb = [this]() {
        auto &s = GetDebugState();
        s.debugDraw = !s.debugDraw;
    }};

    Keybind mineBlock{.btn = MouseButton::LMB, .trigger = InputTrigger::CONTINUOS, .cb = [this]() {
        auto &hit = m_GameState.inReachBlock;

        if (hit.has_value()) {
            if (m_GameState.blockMineTimer < m_GameState.blockMineTimeout) {
                m_GameState.blockMineTimer += m_Window.GetDeltaTime();
                return;
            }
            m_GameState.blockMineTimer = 0.0f;

            if (auto chunk = m_World.TryGetChunk(hit->chunkPos)) {
                chunk->SetBlockBreakStage(
                    hit->blockPos, chunk->GetBlockBreakStage(hit->blockPos) + m_GameState.blockBreakStagesPerMine);
                chunk->isDirty = true;
            }
        }
    }};

    Keybind placeTorch{.btn = MouseButton::MMB, .cb = [this]() {
        auto &hit = m_GameState.inReachBlock;
        if (hit.has_value()) {
            if (auto chunk = m_World.TryGetChunk(hit->chunkPos)) {
                auto b = chunk->GetBlockRaw(hit->blockPos);
                b.lightEmit = 7;
                chunk->SetBlock(hit->blockPos, b);
            }
        }
    }};
    Keybind placeBlock{.btn = MouseButton::RMB, .cb = [this]() {
        auto &hit = m_GameState.inReachBlock;

        if (hit.has_value()) {
            auto worldPos =
                glm::vec3(hit->chunkPos) * static_cast<float>(CHUNK_SIZE) + glm::vec3(hit->blockPos) + hit->normal;
            auto [chunkPos, blockPos] = WorldPos2ChunkAndBlock(worldPos);
            glm::vec3 minA, maxA, minB, maxB;
            blockPos.GetAABB(minA, maxA);
            m_Player.GetAABB(minB, maxB);

            auto chunk = m_World.TryGetChunk(chunkPos);
            if (!AABBIntersects(minA, maxA, minB, maxB) && chunk) {
                chunk->SetBlock(blockPos, {.id = currentBlock});
                spdlog::info("Placed block at C({},{},{}) B({},{},{})", chunkPos.x, chunkPos.y, chunkPos.z,
                    blockPos.v.x, blockPos.v.y, blockPos.v.z);
            }
        }
    }};

    Keybind toggleFly{.key = KeyboardKey::SPACE, .trigger = InputTrigger::DOUBLE, .cb = [this]() {
        m_Player.SetFlight(!m_Player.GetFlight());
    }};

    input.RegisterKeybind(toggleWireframe);
    input.RegisterKeybind(toggleDebugDraw);
    input.RegisterKeybind(toggleFly);
    input.RegisterKeybind(mineBlock);
    input.RegisterKeybind(placeTorch);
    input.RegisterKeybind(placeBlock);

    nvgCreateFont(vg, "roboto", "/home/focus/Downloads/Roboto-Regular.ttf");

    while (!m_Window.ShouldClose()) {
        m_Window.Begin();

        glm::vec3 start = m_Player.GetEyePosition();
        glm::vec3 end = start + m_Player.GetLookDirection() * m_GameState.reach;
        if (auto hit = m_World.CastRay(start, end)) {
            glm::ivec3 worldPos = (hit->chunkPos * static_cast<int>(CHUNK_SIZE)) + glm::ivec3(hit->blockPos);
            auto [chunkPos, blockPos] = WorldPos2ChunkAndBlock(worldPos);
            if (m_World.TryGetChunk(chunkPos)) {
                m_GameState.inReachBlock = hit;
            }
        } else {
            m_GameState.inReachBlock = std::nullopt;
        }

        if (m_Window.IsResized()) {
            glViewport(0, 0, m_Window.GetWidth(), m_Window.GetHeight());
        }

        if (input.GetScrollDelta() != 0) {
            int off = static_cast<int>(input.GetScrollDelta());
            spdlog::info("{}", off);
            currentBlock = std::clamp(currentBlock + off, 1, static_cast<int>(maxBlock) - 1);
        }

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glDepthMask(GL_TRUE);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glDisable(GL_STENCIL_TEST);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        float dt = m_Window.GetDeltaTime();
        auto &camera = m_Player.GetCamera();
        m_Player.Update(dt, m_World, input);
        camera.ProcessMouse(input.GetMouseDelta());

        m_World.Update();
        m_World.Render(camera);

        if (m_DebugState.debugDraw) {
            m_DebugRenderer.Render(camera, dt);
        }

        if (m_GameState.inReachBlock.has_value()) {
            auto &hit = *m_GameState.inReachBlock;
            float alpha = 0.2f;
            m_BlockOverlayRenderer.DrawBox(
                (glm::vec3(hit.chunkPos) * static_cast<float>(CHUNK_SIZE) + glm::vec3(hit.blockPos)),
                {0.0f, 0.0f, 0.0f, alpha});
        }
        m_BlockOverlayRenderer.Render(camera);
        {
            // crosshair
            nvgBeginFrame(vg, m_Window.GetWidth(), m_Window.GetHeight(), 2.0);
            nvgBeginPath(vg);
            nvgCircle(vg, m_Window.GetWidth() / 2.0f, m_Window.GetHeight() / 2.0f, 5);
            nvgFillColor(vg, nvgRGBA(255, 0, 0, 255));
            nvgFill(vg);

            // coords
            nvgBeginPath(vg);
            auto p = m_Player.GetPosition();
            nvgFontSize(vg, 20);
            nvgFontFace(vg, "roboto");
            nvgFillColor(vg, nvgRGB(255, 255, 255));
            nvgTextAlign(vg, NVG_ALIGN_CENTER | NVG_ALIGN_TOP);
            nvgText(vg, m_Window.GetWidth() / 2.0f, 5, std::format("({:.1f},{:.1f},{:.1f})", p.x, p.y, p.z).c_str(),
                nullptr);
            nvgTextAlign(vg, NVG_ALIGN_RIGHT | NVG_ALIGN_TOP);

            nvgText(vg, m_Window.GetWidth() - 5, 30,
                std::format("Selected Block: {}", BlockRegistry::GetInstance().GetName(currentBlock)).c_str(), nullptr);

            // waila
            if (m_GameState.inReachBlock.has_value()) {
                auto &hit = *m_GameState.inReachBlock;
                nvgBeginPath(vg);
                nvgFontSize(vg, 20);
                nvgFontFace(vg, "roboto");
                nvgFillColor(vg, nvgRGB(255, 255, 255));
                nvgTextAlign(vg, NVG_ALIGN_RIGHT | NVG_ALIGN_TOP);
                nvgText(vg, m_Window.GetWidth() - 5, 5,
                    std::format("{} id: {} lightemit: {} lightlv: {} breakstage: {}", hit.block.name, hit.block.id,
                        hit.block.lightEmit, hit.block.lightLv, hit.block.breakStage)
                        .c_str(),
                    nullptr);
            }
            nvgEndFrame(vg);
        }

        m_Window.End();
    }

    if (vg) {
        nvgDeleteGL3(vg);
        vg = nullptr;
    }
}
