#include "Enemy.h"

Enemy::Enemy(const std::string& name, int hp, int damage,
             const std::string& battle_desc)
    : name_(name), hp_(hp), damage_(damage), battle_desc_(battle_desc) {}

std::string Enemy::getName() const { return name_; }
int Enemy::getHp() const { return hp_; }
int Enemy::getDamage() const { return damage_; }
std::string Enemy::getBattleDesc() const { return battle_desc_; }
void Enemy::takeDamage(int amount) { hp_ -= amount; }
bool Enemy::isAlive() const { return hp_ > 0; }
