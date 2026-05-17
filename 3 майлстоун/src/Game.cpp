#include "Game.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_map>

#include "Combat.h"
#include "utils.h"

#ifdef _WIN32
#include <windows.h>
#endif

namespace {
std::string extractArgument(const std::string& cmd) {
  size_t space_pos = cmd.find(' ');
  if (space_pos == std::string::npos) return "";
  std::string arg = cmd.substr(space_pos + 1);
  size_t start = arg.find_first_not_of(" \t");
  if (start == std::string::npos) return "";
  return arg.substr(start);
}
}  // namespace

using namespace constants;

namespace {
// Карта команд (английские и русские)
const std::unordered_map<std::string, Command> kCommandMap = {
    {"help", Command::kHelp},           {"помощь", Command::kHelp},
    {"look", Command::kLook},           {"осмотреться", Command::kLook},
    {"north", Command::kNorth},         {"север", Command::kNorth},
    {"south", Command::kSouth},         {"юг", Command::kSouth},
    {"east", Command::kEast},           {"восток", Command::kEast},
    {"west", Command::kWest},           {"запад", Command::kWest},
    {"inventory", Command::kInventory}, {"inv", Command::kInventory},
    {"инвентарь", Command::kInventory}, {"инв", Command::kInventory},
    {"exit", Command::kExit},           {"выход", Command::kExit},
    {"quit", Command::kQuit},           {"выйти", Command::kQuit},
    {"stats", Command::kStats},         {"статистика", Command::kStats},
    {"save", Command::kSave},           {"сохранить", Command::kSave},
    {"load", Command::kLoad},           {"загрузить", Command::kLoad}};
}  // namespace

Game::Game() : player_(kStartHp), running_(true), in_combat_(false) {
  try {
    initWorld();
  } catch (const std::exception& e) {
    std::cerr << "Критическая ошибка при загрузке мира: " << e.what()
              << std::endl;
    running_ = false;
    return;
  }
  clearScreen();
  std::cout << "Добро пожаловать в игру!\n";
  look();
}

void Game::initWorld() {
  rooms_ = WorldLoader::loadRooms(kRoomsFile);
  auto all_items = WorldLoader::loadItems(kItemsFile);
  auto all_enemies = WorldLoader::loadEnemies(kEnemiesFile);

  // Распределение предметов и врагов по комнатам
  for (auto& room : rooms_) {
    switch (room->getId()) {
      case 1: {  // Зал стражей
        auto skel =
            std::find_if(all_enemies.begin(), all_enemies.end(),
                         [](auto e) { return e->getName() == "Скелет"; });
        if (skel != all_enemies.end()) room->addEnemy(*skel);
        break;
      }
      case 2: {  // Оружейная
        auto rusty =
            std::find_if(all_items.begin(), all_items.end(),
                         [](auto i) { return i->getName() == "Ржавый меч"; });
        if (rusty != all_items.end()) room->addItem(*rusty);
        auto steel =
            std::find_if(all_items.begin(), all_items.end(),
                         [](auto i) { return i->getName() == "Стальной меч"; });
        if (steel != all_items.end()) room->addItem(*steel);
        auto twohanded = std::find_if(
            all_items.begin(), all_items.end(),
            [](auto i) { return i->getName() == "Двуручный меч"; });
        if (twohanded != all_items.end()) room->addItem(*twohanded);
        break;
      }
      case 3: {  // Библиотека
        auto ghost =
            std::find_if(all_enemies.begin(), all_enemies.end(),
                         [](auto e) { return e->getName() == "Призрак"; });
        if (ghost != all_enemies.end()) room->addEnemy(*ghost);

        auto small_potion = std::find_if(
            all_items.begin(), all_items.end(),
            [](auto i) { return i->getName() == "Малое зелье здоровья"; });
        if (small_potion != all_items.end()) room->addItem(*small_potion);

        auto medium_potion = std::find_if(
            all_items.begin(), all_items.end(),
            [](auto i) { return i->getName() == "Зелье здоровья"; });
        if (medium_potion != all_items.end()) room->addItem(*medium_potion);

        auto big_potion = std::find_if(
            all_items.begin(), all_items.end(),
            [](auto i) { return i->getName() == "Большое зелье здоровья"; });
        if (big_potion != all_items.end()) room->addItem(*big_potion);

        break;
      }
      case 4: {  // Тронный зал
        auto lich =
            std::find_if(all_enemies.begin(), all_enemies.end(),
                         [](auto e) { return e->getName() == "Король-Лич"; });
        if (lich != all_enemies.end()) room->addEnemy(*lich);
        auto crown =
            std::find_if(all_items.begin(), all_items.end(),
                         [](auto i) { return i->getName() == "Корона"; });
        if (crown != all_items.end()) room->addItem(*crown);
        break;
      }
      default:
        break;
    }
  }
  current_room_ = rooms_.empty() ? nullptr : rooms_[0];
  if (!current_room_) throw std::runtime_error("Не найдена стартовая комната.");
}

void Game::run() {
  std::string input;
  while (running_ && player_.isAlive()) {
    if (!in_combat_) {
      std::cout << "> ";
      std::getline(std::cin, input);
      processCommand(input);
    } else {
      bool victory = Combat::startCombat(player_, current_enemy_);
      if (victory) {
        defeated_enemies_.push_back(current_enemy_->getName());
        current_room_->removeEnemy(current_enemy_->getName());
        in_combat_ = false;
        current_enemy_ = nullptr;
        std::cout << "\nВы победили! Можете продолжить исследование.\n";
        utils::waitForEnter();
        clearScreen();
        look();
      } else {
        if (!player_.isAlive()) {
          std::cout << "Игра окончена. Вы погибли.\n";
          running_ = false;
        } else {
          in_combat_ = false;
          current_enemy_ = nullptr;
          std::cout << "\nВы сбежали из боя.\n";
          utils::waitForEnter();
          clearScreen();
          look();
        }
      }
    }
  }
  if (!player_.isAlive())
    std::cout << "GAME OVER\n";
  else
    std::cout << "Спасибо за игру!\n";
}

Command Game::parseCommand(const std::string& cmd) const {
  std::string lower_cmd = utils::toLower(cmd);
  if (lower_cmd.rfind("take ", 0) == 0 || lower_cmd.rfind("взять ", 0) == 0)
    return Command::kTake;
  if (lower_cmd.rfind("use ", 0) == 0 ||
      lower_cmd.rfind("использовать ", 0) == 0)
    return Command::kUse;
  auto it = kCommandMap.find(lower_cmd);
  return (it != kCommandMap.end()) ? it->second : Command::kUnknown;
}

void Game::processCommand(const std::string& cmd) {
  if (!in_combat_) clearScreen();
  Command command = parseCommand(cmd);

  switch (command) {
    case Command::kHelp:
      showHelp();
      break;
    case Command::kLook:
      look();
      break;
    case Command::kNorth:
      move("north");
      break;
    case Command::kSouth:
      move("south");
      break;
    case Command::kEast:
      move("east");
      break;
    case Command::kWest:
      move("west");
      break;
    case Command::kInventory:
      inventory();
      break;
    case Command::kTake: {
      std::string arg = extractArgument(cmd);
      if (arg.empty()) {
        std::cout
            << "Что взять? Укажите название предмета.\nПример: take Меч\n";
      } else {
        take(arg);
      }
      break;
    }
    case Command::kUse: {
      std::string arg = extractArgument(cmd);
      if (arg.empty()) {
        std::cout << "Что использовать? Укажите название предмета.\nПример: "
                     "use Зелье\n";
      } else {
        use(arg);
      }
      break;
    }
    case Command::kExit:
      exitGame();
      break;
    case Command::kQuit:
      running_ = false;
      break;
    case Command::kStats:
      showStats();
      break;
    case Command::kSave:
      saveGame(constants::kSaveFile);
      break;
    case Command::kLoad:
      loadGame(constants::kSaveFile);
      break;
    case Command::kUnknown:
      std::cout << "Неизвестная команда. Введите 'help' (помощь).\n";
      break;
  }
}

void Game::showHelp() {
  std::cout << "Доступные команды (английские или русские):\n";
  std::cout << "  look / осмотреться\n";
  std::cout << "  north / south / east / west  (север / юг / восток / запад)\n";
  std::cout << "  take <предмет> / взять <предмет>\n";
  std::cout << "  inventory / inv / инвентарь\n";
  std::cout << "  use <предмет> / использовать <предмет>\n";
  std::cout << "  stats / статистика\n";
  std::cout << "  save / сохранить\n";
  std::cout << "  load / загрузить\n";
  std::cout << "  exit / выход  (победа, если есть корона и вы на входе)\n";
  std::cout << "  quit / выйти  (без победы)\n";
}

void Game::showStats() {
  std::cout << "\n=== Характеристики ===\n";
  std::cout << "Здоровье: " << player_.getHp() << " / " << player_.getMaxHp()
            << "\n";
  int total_damage = kBaseDamage + player_.getInventory().getWeaponBonus();
  std::cout << "Базовый урон: " << kBaseDamage;
  int bonus = player_.getInventory().getWeaponBonus();
  if (bonus > 0) {
    std::cout << " + " << bonus << " (от оружия) = " << total_damage;
  }
  std::cout << "\n";
  auto weapon = player_.getInventory().getWeapon();
  if (weapon)
    std::cout << "Экипировано: " << weapon->getName() << "\n";
  else
    std::cout << "Оружие не экипировано\n";
  std::cout << "Предметов в инвентаре: " << player_.getInventory().getCount()
            << "\n";
  std::cout << "==================\n";
}

void Game::look() {
  std::cout
      << "=== ПОДСКАЗКА: введите 'help' (помощь) для списка команд ===\n\n";

  std::cout << "=== " << current_room_->getName() << " ===\n";
  std::cout << current_room_->getDescription() << "\n";

  const auto& items = current_room_->getItems();
  if (!items.empty()) {
    std::cout << "Вы видите: ";
    for (const auto& item : items) std::cout << item->getName() << " ";
    std::cout << "\n";
  }

  const auto& enemies = current_room_->getEnemies();
  if (!enemies.empty()) {
    std::cout << "Здесь есть враги: ";
    for (const auto& enemy : enemies) std::cout << enemy->getName() << " ";
    std::cout << "\n";
    if (!in_combat_) {
      in_combat_ = true;
      current_enemy_ = enemies[0];
      std::cout << "На вас нападает " << current_enemy_->getName() << "!\n";
    }
  }

  std::cout << "Выходы: ";
  for (const auto& dir : {"north", "south", "east", "west"}) {
    if (current_room_->getExit(dir).has_value()) {
      std::string ru;
      if (dir == "north")
        ru = "север";
      else if (dir == "south")
        ru = "юг";
      else if (dir == "east")
        ru = "восток";
      else
        ru = "запад";
      std::cout << dir << " (" << ru << ") ";
    }
  }
  std::cout << "\n";
}

void Game::move(const std::string& direction) {
  if (in_combat_) {
    std::cout << "Вы в бою! Нельзя перемещаться.\n";
    return;
  }
  auto next_id_opt = current_room_->getExit(direction);
  if (!next_id_opt.has_value()) {
    std::cout << "Туда нельзя пойти (нет прохода).\n";
    return;
  }
  int next_id = next_id_opt.value();
  auto it = std::find_if(rooms_.begin(), rooms_.end(),
                         [next_id](auto r) { return r->getId() == next_id; });
  if (it != rooms_.end()) {
    current_room_ = *it;
    look();
  } else {
    std::cout << "Ошибка: комната не найдена.\n";
  }
}

void Game::take(const std::string& item_name) {
  if (in_combat_) {
    std::cout << "Вы в бою! Сначала завершите бой.\n";
    return;
  }
  if (item_name.empty()) {
    std::cout << "Укажите, что взять.\n";
    return;
  }
  auto& items = current_room_->getItems();
  auto it = std::find_if(items.begin(), items.end(), [&](auto i) {
    return utils::toLower(i->getName()) == utils::toLower(item_name);
  });
  if (it != items.end()) {
    player_.getInventory().addItem(*it);
    items.erase(it);
  } else {
    std::cout << "Здесь нет такого предмета.\n";
  }
}

void Game::use(const std::string& item_name) {
  if (in_combat_) {
    std::cout << "В бою используйте 'use <предмет>' как часть боевых команд.\n";
    return;
  }
  if (item_name.empty()) {
    std::cout << "Укажите, что использовать.\n";
    return;
  }
  auto item = player_.getInventory().findItem(item_name);
  if (!item) {
    // поиск без учёта регистра
    const auto& items = player_.getInventory().getItems();
    auto it = std::find_if(items.begin(), items.end(), [&](auto i) {
      return utils::toLower(i->getName()) == utils::toLower(item_name);
    });
    if (it != items.end()) item = *it;
  }
  if (!item) {
    std::cout << "У вас нет такого предмета.\n";
    return;
  }
  if (item->getType() == ItemType::kConsumable) {
    player_.heal(item->getModifier());
    player_.getInventory().removeItem(item->getName());
  } else if (item->getType() == ItemType::kWeapon) {
    player_.getInventory().setWeapon(item->getName());
  } else {
    std::cout << "Этот предмет нельзя использовать.\n";
  }
}

void Game::inventory() {
  player_.getInventory().listItems();
  auto weapon = player_.getInventory().getWeapon();
  if (weapon)
    std::cout << "Экипировано оружие: " << weapon->getName() << "\n";
  else
    std::cout << "Оружие не экипировано.\n";
}

void Game::exitGame() {
  if (player_.getInventory().hasItem("Корона") &&
      current_room_->getName() == "Вход в склеп") {
    std::cout << "Вы выходите из склепа с короной в руках. Легенды будут "
                 "слагаться о вашем подвиге! ПОБЕДА!\n";
    running_ = false;
  } else {
    std::cout
        << "Вы не можете выйти: либо у вас нет короны, либо вы не у выхода.\n";
  }
}

void Game::clearScreen() const {
#ifdef _WIN32
  system("cls");
#else
  system("clear");
#endif
}

void Game::saveGame(const std::string& filename) {
  std::ofstream file(filename);
  if (!file.is_open()) {
    std::cout << "Не удалось сохранить игру.\n";
    return;
  }
  file << "current_room_id=" << current_room_->getId() << "\n";
  file << "player_hp=" << player_.getHp() << "\n";
  file << "inventory_items=";
  for (const auto& item : player_.getInventory().getItems()) {
    file << item->getName() << ";";
  }
  file << "\n";
  auto weapon = player_.getInventory().getWeapon();
  file << "equipped_weapon=" << (weapon ? weapon->getName() : "") << "\n";
  file << "defeated_enemies=";
  for (const auto& name : defeated_enemies_) {
    file << name << ";";
  }
  file << "\n";
  std::cout << "Игра сохранена в " << filename << "\n";
}

void Game::loadGame(const std::string& filename) {
  std::ifstream file(filename);
  if (!file.is_open()) {
    std::cout << "Файл сохранения не найден.\n";
    return;
  }
  std::string line;
  int new_room_id = -1;
  int new_hp = kStartHp;
  std::vector<std::string> inventory_names;
  std::string equipped_weapon_name;
  std::vector<std::string> defeated_names;

  while (std::getline(file, line)) {
    std::istringstream iss(line);
    std::string key;
    if (std::getline(iss, key, '=')) {
      std::string value;
      std::getline(iss, value);
      if (key == "current_room_id")
        new_room_id = std::stoi(value);
      else if (key == "player_hp")
        new_hp = std::stoi(value);
      else if (key == "inventory_items") {
        std::stringstream ss(value);
        std::string item;
        while (std::getline(ss, item, ';')) {
          if (!item.empty()) inventory_names.push_back(item);
        }
      } else if (key == "equipped_weapon")
        equipped_weapon_name = value;
      else if (key == "defeated_enemies") {
        std::stringstream ss(value);
        std::string enemy;
        while (std::getline(ss, enemy, ';')) {
          if (!enemy.empty()) defeated_names.push_back(enemy);
        }
      }
    }
  }

  // Восстановление игрока
  player_ = Player(kStartHp);
  int diff = new_hp - player_.getHp();
  if (diff > 0)
    player_.heal(diff);
  else if (diff < 0)
    player_.takeDamage(-diff);

  // Восстановление инвентаря
  auto all_items = WorldLoader::loadItems(kItemsFile);
  Inventory new_inv;
  for (const auto& name : inventory_names) {
    auto it = std::find_if(all_items.begin(), all_items.end(),
                           [&](auto i) { return i->getName() == name; });
    if (it != all_items.end()) new_inv.addItem(*it);
  }
  if (!equipped_weapon_name.empty()) new_inv.setWeapon(equipped_weapon_name);
  player_.getInventory() = new_inv;

  defeated_enemies_ = defeated_names;
  for (auto& room : rooms_) {
    for (const auto& name : defeated_enemies_) {
      room->removeEnemy(name);
    }
  }

  auto it = std::find_if(rooms_.begin(), rooms_.end(), [new_room_id](auto r) {
    return r->getId() == new_room_id;
  });
  current_room_ = (it != rooms_.end()) ? *it : rooms_[0];
  in_combat_ = false;
  current_enemy_ = nullptr;
  std::cout << "Игра загружена из " << filename << "\n";
  look();
}
