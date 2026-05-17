#ifndef ITEM_H
#define ITEM_H

#include <string>

enum class ItemType { kWeapon, kConsumable, kKey };

class Item {
 public:
  Item(const std::string& name, ItemType type, int modifier,
       const std::string& desc);

  std::string getName() const;
  ItemType getType() const;
  int getModifier() const;
  std::string getDescription() const;

 private:
  std::string name_;
  ItemType type_;
  int modifier_;
  std::string description_;
};

#endif
