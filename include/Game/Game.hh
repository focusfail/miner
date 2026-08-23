#pragma once

#include "Player/Player.hh"
#include "Render/BlockOverlayRenderer.hh"
#include "Render/Debug/DebugRenderer.hh"
#include "TaskScheduler.h"

#include "Platform/Window.hh"
#include "Util/NonCopyable.hh"
#include "Util/NonMoveable.hh"
#include "World/World.hh"
#include <optional>

struct DebugState {
    bool drawWireframe = false;
    bool debugDraw = false;
};

struct GameState {
    int blockBreakStagesPerMine = 1;
    float blockMineTimeout = 0.05f;
    float blockMineTimer = 0.0f;
    float reach = 4.0f;
    std::optional<HitResult> inReachBlock = std::nullopt;
};

class Game : private NonCopyable, private NonMovable {
public:
    static Game &GetInstance() {
        static Game instance;
        return instance;
    }

    void Init();
    void Mainloop();
    void Shutdown();

    Window &GetGameWindow() { return m_Window; };
    DebugState &GetDebugState() { return m_DebugState; }

private:
    Window m_Window;
    World m_World;
    Player m_Player;
    DebugRenderer m_DebugRenderer;
    BlockOverlayRenderer m_BlockOverlayRenderer;
    DebugState m_DebugState;
    GameState m_GameState;
    enki::TaskScheduler m_Sched;
};