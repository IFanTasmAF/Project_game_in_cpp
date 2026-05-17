#include <clocale>

#include "Game.h"
#ifdef _WIN32
#include <windows.h>
#endif

int main() {
#ifdef _WIN32
  SetConsoleCP(1251);
  SetConsoleOutputCP(1251);
#endif
  std::setlocale(LC_ALL, "rus");

  Game game;
  game.run();
  return 0;
}
