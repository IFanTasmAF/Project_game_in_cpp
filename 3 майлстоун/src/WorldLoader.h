#ifndef WORLD_LOADER_H
#define WORLD_LOADER_H

#include <memory>
#include <string>
#include <vector>

#include "Enemy.h"
#include "Item.h"
#include "Room.h"

class WorldLoader {
 public:
  static std::vector<std::shared_ptr<Room>> loadRooms(
      const std::string& filename);
  static std::vector<std::shared_ptr<Item>> loadItems(
      const std::string& filename);
  static std::vector<std::shared_ptr<Enemy>> loadEnemies(
      const std::string& filename);
};

#endif
