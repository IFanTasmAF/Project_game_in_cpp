#include "Inventory.h"

#include <algorithm>
#include <iostream>

void Inventory::addItem(std::shared_ptr<Item> item) {
  items_.push_back(item);
  std::cout << "Вы взяли: " << item->getName() << "\n";
}

bool Inventory::removeItem(const std::string& name) {
  auto it = std::find_if(
      items_.begin(), items_.end(),
      [&](const std::shared_ptr<Item>& i) { return i->getName() == name; });
  if (it != items_.end()) {
    items_.erase(it);
    return true;
  }
  return false;
}

std::shared_ptr<Item> Inventory::findItem(const std::string& name) const {
  auto it = std::find_if(
      items_.begin(), items_.end(),
      [&](const std::shared_ptr<Item>& i) { return i->getName() == name; });
  if (it != items_.end()) return *it;
  return nullptr;
}

void Inventory::listItems() const {
  if (items_.empty()) {
    std::cout << "Инвентарь пуст.\n";
  } else {
    std::cout << "Ваш инвентарь:\n";
    for (const auto& item : items_) {
      std::cout << "- " << item->getName() << " (" << item->getDescription()
                << ")\n";
    }
  }
}

bool Inventory::hasItem(const std::string& name) const {
  return findItem(name) != nullptr;
}

int Inventory::getWeaponBonus() const {
  if (equipped_weapon_ && equipped_weapon_->getType() == ItemType::kWeapon)
    return equipped_weapon_->getModifier();
  return 0;
}

void Inventory::setWeapon(const std::string& name) {
  auto item = findItem(name);
  if (item && item->getType() == ItemType::kWeapon) {
    equipped_weapon_ = item;
    std::cout << "Вы экипировали " << name << ".\n";
  } else {
    std::cout << "Невозможно экипировать " << name << ".\n";
  }
}

std::shared_ptr<Item> Inventory::getWeapon() const { return equipped_weapon_; }
