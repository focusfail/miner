#pragma once

#include "Assets/AssetsManager.hh"
#include "Player/Player.hh"
#include "Render/Debug/DebugRenderer.hh"
#include "TaskScheduler.h"
#include <memory>

#include "Platform/Window.hh"
#include "Util/NonCopyable.hh"
#include "Util/NonMoveable.hh"
#include "World/World.hh"

struct DebugState {
    bool drawWireframe = false;
    bool debugDraw = false;
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

    std::weak_ptr<Window> GetGameWindow() const;
    DebugState &GetDebugState() { return m_DebugState; }

  private:
    std::shared_ptr<Window> m_Window;
    World m_World;
    Player m_Player;
    DebugRenderer m_DebugRenderer;
    DebugState m_DebugState;
    enki::TaskScheduler m_Sched;
};