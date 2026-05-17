#include "Player.h"

#include <iostream>

Player::Player(int max_hp) : hp_(max_hp), max_hp_(max_hp) {}

int Player::getHp() const { return hp_; }
int Player::getMaxHp() const { return max_hp_; }

void Player::heal(int amount) {
  hp_ += amount;
  if (hp_ > max_hp_) hp_ = max_hp_;
  std::cout << "Вы восстановили " << amount << " HP. Текущее HP: " << hp_
            << "\n";
}

void Player::takeDamage(int amount) {
  hp_ -= amount;
  std::cout << "Вы получили " << amount << " урона. Осталось HP: " << hp_
            << "\n";
}

bool Player::isAlive() const { return hp_ > 0; }

Inventory& Player::getInventory() { return inventory_; }

const Inventory& Player::getInventory() const { return inventory_; }
