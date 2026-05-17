#ifndef COMBAT_H
#define COMBAT_H

#include <memory>

#include "Enemy.h"
#include "Player.h"

class Combat {
 public:
  // Returns true if player wins, false if fled or died.
  static bool startCombat(Player& player, std::shared_ptr<Enemy> enemy);
};

#endif
