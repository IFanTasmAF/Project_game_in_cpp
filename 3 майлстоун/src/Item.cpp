#include "Item.h"

Item::Item(const std::string& name, ItemType type, int modifier,
           const std::string& desc)
    : name_(name), type_(type), modifier_(modifier), description_(desc) {}

std::string Item::getName() const { return name_; }
ItemType Item::getType() const { return type_; }
int Item::getModifier() const { return modifier_; }
std::string Item::getDescription() const { return description_; }
