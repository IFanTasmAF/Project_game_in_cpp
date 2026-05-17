#ifndef ROOM_H
#define ROOM_H

#include <map>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "Enemy.h"
#include "Item.h"

class Room {
 public:
  Room(int id, const std::string& name, const std::string& description);

  int getId() const;
  std::string getName() const;
  std::string getDescription() const;
  void setExit(const std::string& direction, int room_id);
  std::optional<int> getExit(
      const std::string& direction) const;

  void addEnemy(std::shared_ptr<Enemy> enemy);
  void addItem(std::shared_ptr<Item> item);
  const std::vector<std::shared_ptr<Enemy>>& getEnemies() const;
  std::vector<std::shared_ptr<Item>>& getItems();
  void removeEnemy(const std::string& name);
  void removeItem(const std::string& name);

 private:
  int id_;
  std::string name_;
  std::string description_;
  std::map<std::string, int> exits_;
  std::vector<std::shared_ptr<Enemy>> enemies_;
  std::vector<std::shared_ptr<Item>> items_;
};

#endif
