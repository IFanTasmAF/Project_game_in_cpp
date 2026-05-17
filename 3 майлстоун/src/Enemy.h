#ifndef ENEMY_H
#define ENEMY_H

#include <string>

class Enemy {
 public:
  Enemy(const std::string& name, int hp, int damage,
        const std::string& battle_desc);

  std::string getName() const;
  int getHp() const;
  int getDamage() const;
  std::string getBattleDesc() const;
  void takeDamage(int amount);
  bool isAlive() const;

 private:
  std::string name_;
  int hp_;
  int damage_;
  std::string battle_desc_;
};

#endif
