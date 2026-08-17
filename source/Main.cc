#include "Game/Game.hh"

int main() {
  auto &game = Game::GetInstance();
  game.Init();
  game.Mainloop();
}