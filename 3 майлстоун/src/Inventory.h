#ifndef INVENTORY_H
#define INVENTORY_H

#include <memory>
#include <string>
#include <vector>

#include "Item.h"

class Inventory {
 public:
  void addItem(std::shared_ptr<Item> item);
  bool removeItem(const std::string& name);
  [[nodiscard]] std::shared_ptr<Item> findItem(const std::string& name) const;
  void listItems() const;
  [[nodiscard]] bool hasItem(const std::string& name) const;
  [[nodiscard]] int getWeaponBonus() const;
  void setWeapon(const std::string& name);
  [[nodiscard]] std::shared_ptr<Item> getWeapon() const;
  [[nodiscard]] size_t getCount() const { return items_.size(); }
  [[nodiscard]] const std::vector<std::shared_ptr<Item>>& getItems() const {
    return items_;
  }

 private:
  std::vector<std::shared_ptr<Item>> items_;
  std::shared_ptr<Item> equipped_weapon_;
};

#endif
