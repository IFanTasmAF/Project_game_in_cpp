#include "Room.h"

#include <algorithm>

Room::Room(int id, const std::string& name, const std::string& description)
    : id_(id), name_(name), description_(description) {}

int Room::getId() const { return id_; }
std::string Room::getName() const { return name_; }
std::string Room::getDescription() const { return description_; }

void Room::setExit(const std::string& direction, int room_id) {
  exits_[direction] = room_id;
}

std::optional<int> Room::getExit(const std::string& direction) const {
  auto it = exits_.find(direction);
  if (it != exits_.end()) return it->second;
  return std::nullopt;
}

void Room::addEnemy(std::shared_ptr<Enemy> enemy) { enemies_.push_back(enemy); }
void Room::addItem(std::shared_ptr<Item> item) { items_.push_back(item); }
const std::vector<std::shared_ptr<Enemy>>& Room::getEnemies() const {
  return enemies_;
}
std::vector<std::shared_ptr<Item>>& Room::getItems() { return items_; }

void Room::removeEnemy(const std::string& name) {
  auto it = std::find_if(enemies_.begin(), enemies_.end(),
                         [&](const auto& e) { return e->getName() == name; });
  if (it != enemies_.end()) enemies_.erase(it);
}

void Room::removeItem(const std::string& name) {
  auto it = std::find_if(items_.begin(), items_.end(),
                         [&](const auto& i) { return i->getName() == name; });
  if (it != items_.end()) items_.erase(it);
}
