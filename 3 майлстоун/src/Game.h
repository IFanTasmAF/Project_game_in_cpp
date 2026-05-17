#ifndef GAME_H
#define GAME_H

#include <memory>
#include <string>
#include <vector>

#include "Player.h"
#include "Room.h"
#include "WorldLoader.h"
#include "constants.h"

class Game {
 public:
  Game();
  void run();

 private:
  constants::Command parseCommand(
      const std::string& cmd) const;  // этот может быть const
  void processCommand(const std::string& cmd);
  void showHelp();   // убрали const
  void showStats();  // убрали const
  void look();
  void move(const std::string& direction);
  void take(const std::string& item_name);
  void use(const std::string& item_name);
  void inventory();  // убрали const
  void exitGame();
  void saveGame(const std::string& filename);
  void loadGame(const std::string& filename);
  void clearScreen() const;  // этот может быть const
  void initWorld();

  std::vector<std::shared_ptr<Room>> rooms_;
  std::shared_ptr<Room> current_room_;
  Player player_;
  bool running_;
  bool in_combat_;
  std::shared_ptr<Enemy> current_enemy_;
  std::vector<std::string> defeated_enemies_;
};

#endif
