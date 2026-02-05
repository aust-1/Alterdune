#pragma once

#include <memory>
#include <string>
#include <vector>

#include "../acts/ActCatalog.hpp"
#include "../entities/Monster.hpp"
#include "../items/Inventory.hpp"

class CsvLoader {
public:
  Inventory loadItems(const std::string &rPath) const;
  std::vector<std::unique_ptr<Monster>>
  loadMonsters(const std::string &rPath, const ActCatalog &rCatalog) const;
};
