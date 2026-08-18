#include "Game/Game.hh"
#include "Platform/Input.hh"
#include "Platform/Keys.hh"
#include "Render/Camera.hh"
#include "World/Block/Block.hh"
#include "World/World.hh"
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
  camera.Init({8.5, 3, 8.5});

  m_World.Init();
  auto &input = m_Window->GetInput();
  bool poly = false;

  input.RegisterKeybind(KeyboardKey::F7, [&poly]() {
    glPolygonMode(poly ? GL_FRONT : GL_FRONT_AND_BACK,
                  poly ? GL_FILL : GL_LINE);
    poly = !poly;
  });

  input.RegisterKeybind(
      KeyboardKey::TAB,
      [this, &camera]() {
        glm::vec3 start = camera.GetPosition();
        glm::vec3 end = start + camera.GetLookDirection() * 10000.0f;
        if (auto maybeHit = m_World.CastRay(start, end)) {
          auto &hit = *maybeHit;

          if (auto chunk =
                  m_World.TryGetChunkDataByPosition(hit.chunkPosition)) {
            (*chunk)->SetBlock(hit.blockPosition, 0);
          }
        }
      },
      KeyTrigger::HELD);

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
