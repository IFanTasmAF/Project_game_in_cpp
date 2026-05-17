#include "Combat.h"

#include <algorithm>
#include <iostream>

#include "Combat.h"
#include "constants.h"
#include "utils.h"

bool Combat::startCombat(Player& player, std::shared_ptr<Enemy> enemy) {
  using namespace constants;

  std::cout << "\n=== БОЙ ===\n";
  std::cout << enemy->getBattleDesc() << "\n";

  while (player.isAlive() && enemy->isAlive()) {
    std::cout << "\nВаше HP: " << player.getHp() << " | " << enemy->getName()
              << " HP: " << enemy->getHp() << "\n";
    std::cout << "Введите команду (attack / run / use / help): ";

    std::string cmd;
    std::getline(std::cin, cmd);
    std::string lower_cmd = utils::toLower(cmd);

    if (lower_cmd == "attack" || lower_cmd == "атака") {
      int damage = kBaseDamage + player.getInventory().getWeaponBonus();
      std::cout << "Вы нанесли " << damage << " урона.\n";
      enemy->takeDamage(damage);
      if (!enemy->isAlive()) {
        std::cout << "Вы победили " << enemy->getName() << "!\n";
        return true;
      }
      int enemy_damage = enemy->getDamage();
      player.takeDamage(enemy_damage);
      if (!player.isAlive()) return false;
    } else if (lower_cmd == "run" || lower_cmd == "сбежать" ||
               lower_cmd == "убежать") {
      int chance = utils::getRandomInt(0, 99);
      if (chance < kFleeChancePercent) {
        std::cout << "Вы успешно сбежали!\n";
        return false;
      } else {
        std::cout << "Не удалось сбежать! Враг атакует.\n";
        int enemy_damage = enemy->getDamage();
        player.takeDamage(enemy_damage);
        if (!player.isAlive()) return false;
      }
    } else if (lower_cmd == "help" || lower_cmd == "помощь") {
      std::cout << "В бою доступны команды:\n";
      std::cout << "  attack / атака      - нанести удар\n";
      std::cout << "  run / сбежать       - попытаться убежать (50% шанс)\n";
      std::cout << "  use <предмет> / использовать <предмет> - выпить зелье, "
                   "экипировать оружие\n";
      std::cout << "  help / помощь       - показать эту справку\n";
    } else if (lower_cmd.rfind("use ", 0) == 0 ||
               lower_cmd.rfind("использовать ", 0) == 0) {
      size_t space_pos = cmd.find(' ');
      if (space_pos == std::string::npos) {
        std::cout << "Укажите предмет для использования.\n";
        continue;
      }
      std::string item_name = cmd.substr(space_pos + 1);
      size_t start = item_name.find_first_not_of(" \t");
      if (start != std::string::npos)
        item_name = item_name.substr(start);
      else
        item_name.clear();

      if (item_name.empty()) {
        std::cout << "Укажите предмет для использования.\n";
        continue;
      }

      auto item = player.getInventory().findItem(item_name);
      if (!item) {
        const auto& items = player.getInventory().getItems();
        auto it = std::find_if(
            items.begin(), items.end(), [&](std::shared_ptr<Item> i) {
              return utils::toLower(i->getName()) == utils::toLower(item_name);
            });
        if (it != items.end()) item = *it;
      }

      if (!item) {
        std::cout << "У вас нет такого предмета.\n";
        continue;
      }

      if (item->getType() == ItemType::kConsumable) {
        player.heal(item->getModifier());
        player.getInventory().removeItem(item->getName());
      } else if (item->getType() == ItemType::kWeapon) {
        player.getInventory().setWeapon(item->getName());
      } else {
        std::cout << "Этот предмет нельзя использовать в бою.\n";
      }
    } else {
      std::cout << "Неизвестная команда. Введите 'help' для списка команд.\n";
    }
  }
  return player.isAlive();
}
