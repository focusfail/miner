#include "Game/Game.hh"
#include "Platform/Keys.hh"
#include "Render/Camera.hh"
#include "World/Coordinates.hh"
#include <glad/gl.h>
#include <memory>
#include <spdlog/spdlog.h>

void Game::Init() {
  spdlog::info("[Game]: Init");
  m_Window = Window::Create("mine", 1920, 1080);
  m_Window->CaptureMouse(true);
  gladLoaderLoadGL();
  glViewport(0, 0, m_Window->GetWidth(), m_Window->GetHeight());
  glEnable(GL_DEPTH_TEST);
}

void Game::Mainloop() {
  Camera camera;
  camera.Init({20, 20, -20});

  m_World.Init();
  auto &input = m_Window->GetInput();
  bool poly = false;

  input.RegisterKeybind(KeyboardKey::F7, [&poly]() {
    glPolygonMode(poly ? GL_FRONT : GL_FRONT_AND_BACK,
                  poly ? GL_FILL : GL_LINE);
    poly = !poly;
  });

  input.RegisterKeybind(KeyboardKey::SPACE, [this]() {
    if (auto chunk = m_World.TryGetChunkDataByPosition({0, 0, 0})) {
      (*chunk)->ForceMutable();
    }
  });

  while (!m_Window->ShouldClose()) {
    m_Window->Begin();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    camera.ProcessMovement(
        input.KeyDown(KeyboardKey::W), input.KeyDown(KeyboardKey::S),
        input.KeyDown(KeyboardKey::A), input.KeyDown(KeyboardKey::D),
        input.KeyDown(KeyboardKey::SPACE),
        input.KeyDown(KeyboardKey::LEFT_CONTROL), 0.1);
    camera.ProcessMouse(input.GetMouseDelta());

    m_World.Update();
    m_World.Render(camera);

    m_Window->End();
  }
}

std::weak_ptr<Window> Game::GetGameWindow() const { return m_Window; }
