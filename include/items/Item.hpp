#pragma once

#include <string>

#include "../utils/Types.hpp"

class Player;

class Item {
public:
  Item();
  Item(const std::string &rName, ItemType type, int value);

  const std::string &getName() const;
  ItemType getType() const;
  int getValue() const;

  void apply(Player &rTarget) const;

private:
  std::string mName;
  ItemType mType;
  int mValue;
};
