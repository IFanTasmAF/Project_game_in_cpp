#ifndef PLAYER_H
#define PLAYER_H

#include "Inventory.h"

class Player {
 public:
  explicit Player(int max_hp);

  int getHp() const;
  int getMaxHp() const;
  void heal(int amount);
  void takeDamage(int amount);
  bool isAlive() const;
  Inventory& getInventory();              // неконстантная версия
  const Inventory& getInventory() const;  // константная версия

 private:
  int hp_;
  int max_hp_;
  Inventory inventory_;
};

#endif
