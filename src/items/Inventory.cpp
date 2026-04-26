#include "../../include/items/Inventory.hpp"
#include "../../include/entities/Player.hpp"

int Inventory::getQuantity(const std::string &rName) const {
  auto it = mItems.find(rName);
  if (it == mItems.end()) {
    return 0;
  }
  return it->second;
}

void Inventory::add(const Item &rItem, int qty) {
  if (qty <= 0) {
    return;
  }
  mItemDefs[rItem.getName()] = rItem;
  mItems[rItem.getName()] += qty;
}

bool Inventory::use(const std::string &rName, Player &rTarget) {
  auto it = mItems.find(rName);
  if (it == mItems.end() || it->second <= 0) {
    return false;
  }
  auto defIt = mItemDefs.find(rName);
  if (defIt == mItemDefs.end()) {
    return false;
  }
  defIt->second.apply(rTarget);
  --it->second;
  if (it->second <= 0) {
    mItems.erase(it);
  }
  return true;
}

std::vector<Item> Inventory::listItems() const {
  std::vector<Item> result;
  result.reserve(mItemDefs.size());
  for (const auto &entry : mItemDefs) {
    if (getQuantity(entry.first) > 0) {
      result.push_back(entry.second);
    }
  }
  return result;
}
