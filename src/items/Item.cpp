#include "../../include/items/Item.hpp"
#include "../../include/entities/Player.hpp"

Item::Item() : mName(""), mType(ItemType::HEAL), mValue(0) {}

Item::Item(const std::string &rName, ItemType type, int value)
    : mName(rName), mType(type), mValue(value) {}

const std::string &Item::getName() const { return mName; }

ItemType Item::getType() const { return mType; }

int Item::getValue() const { return mValue; }

void Item::apply(Player &rTarget) const {
  if (mType == ItemType::HEAL) {
    rTarget.heal(mValue);
  }
}
