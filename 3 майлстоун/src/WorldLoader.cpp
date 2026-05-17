#include "WorldLoader.h"

#include <fstream>
#include <iostream>
#include <map>
#include <sstream>

#include "json.hpp"

using json = nlohmann::json;

std::vector<std::shared_ptr<Room>> WorldLoader::loadRooms(
    const std::string& filename) {
  std::vector<std::shared_ptr<Room>> rooms;
  std::ifstream file(filename);
  if (!file.is_open()) {
    throw std::runtime_error("Не удалось открыть файл комнат: " + filename);
  }

  std::string line;
  int id = -1;
  std::string name, desc;
  std::map<std::string, int> exits;

  auto commitRoom = [&]() {
    if (id != -1) {
      auto room = std::make_shared<Room>(id, name, desc);
      for (const auto& [dir, target] : exits) {
        room->setExit(dir, target);
      }
      rooms.push_back(room);
      id = -1;
      name.clear();
      desc.clear();
      exits.clear();
    }
  };

  while (std::getline(file, line)) {
    if (line.empty() || line[0] == '#') continue;
    if (line == "---") {
      commitRoom();
      continue;
    }
    std::istringstream iss(line);
    std::string key;
    if (std::getline(iss, key, '=')) {
      std::string value;
      std::getline(iss, value);
      if (key == "id")
        id = std::stoi(value);
      else if (key == "name")
        name = value;
      else if (key == "desc")
        desc = value;
      else if (key == "north")
        exits["north"] = std::stoi(value);
      else if (key == "south")
        exits["south"] = std::stoi(value);
      else if (key == "east")
        exits["east"] = std::stoi(value);
      else if (key == "west")
        exits["west"] = std::stoi(value);
    }
  }
  commitRoom();
  return rooms;
}

std::vector<std::shared_ptr<Item>> WorldLoader::loadItems(
    const std::string& filename) {
  std::vector<std::shared_ptr<Item>> items;
  std::ifstream file(filename);
  if (!file.is_open()) {
    throw std::runtime_error("Не удалось открыть файл предметов: " + filename);
  }
  try {
    json data;
    file >> data;
    for (const auto& entry : data) {
      std::string name = entry["name"];
      std::string type_str = entry["type"];
      int modifier = entry["modifier"];
      std::string desc = entry["description"];
      ItemType type;
      if (type_str == "weapon")
        type = ItemType::kWeapon;
      else if (type_str == "consumable")
        type = ItemType::kConsumable;
      else
        type = ItemType::kKey;
      items.push_back(std::make_shared<Item>(name, type, modifier, desc));
    }
  } catch (const json::parse_error& e) {
    throw std::runtime_error("Ошибка парсинга JSON: " + std::string(e.what()));
  }
  return items;
}

std::vector<std::shared_ptr<Enemy>> WorldLoader::loadEnemies(
    const std::string& filename) {
  std::vector<std::shared_ptr<Enemy>> enemies;
  std::ifstream file(filename);
  if (!file.is_open()) {
    throw std::runtime_error("Не удалось открыть файл врагов: " + filename);
  }
  std::string line;
  while (std::getline(file, line)) {
    if (line.empty() || line[0] == '#') continue;
    std::istringstream iss(line);
    std::string name, battle_desc;
    int hp, damage;
    std::getline(iss, name, ',');
    iss >> hp;
    iss.ignore();
    iss >> damage;
    iss.ignore();
    std::getline(iss, battle_desc);
    enemies.push_back(std::make_shared<Enemy>(name, hp, damage, battle_desc));
  }
  return enemies;
}
