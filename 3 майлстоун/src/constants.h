#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <string>

namespace constants {

// Пути к файлам данных
inline const std::string kRoomsFile = "data/rooms.txt";
inline const std::string kItemsFile = "data/items.json";
inline const std::string kEnemiesFile = "data/enemies.txt";
inline const std::string kSaveFile = "savegame.txt";

// Игровые параметры
inline constexpr int kStartHp = 100;
inline constexpr int kBaseDamage = 10;
inline constexpr int kFleeChancePercent = 50;  // 50%

// Команды
enum class Command {
  kHelp,
  kLook,
  kNorth,
  kSouth,
  kEast,
  kWest,
  kInventory,
  kTake,
  kUse,
  kExit,
  kQuit,
  kStats,
  kSave,
  kLoad,
  kUnknown
};

}  // namespace constants

#endif
