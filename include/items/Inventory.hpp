#pragma once

#include <map>
#include <string>
#include <vector>

#include "Item.hpp"

class Inventory {
public:
  int getQuantity(const std::string &rName) const;
  void add(const Item &rItem, int qty);
  bool use(const std::string &rName, Player &rTarget);
  std::vector<Item> listItems() const;

private:
  std::map<std::string, int> mItems;
  std::map<std::string, Item> mItemDefs;
};
