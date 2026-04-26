#include "../../include/io/CsvLoader.hpp"

#include <cctype>
#include <cstddef>
#include <fstream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "../../include/entities/BossMonster.hpp"
#include "../../include/entities/MiniBossMonster.hpp"
#include "../../include/entities/NormalMonster.hpp"

namespace {
std::string trim(const std::string &rValue) {
  size_t start = 0;
  while (start < rValue.size() &&
         std::isspace(static_cast<unsigned char>(rValue[start]))) {
    ++start;
  }
  size_t end = rValue.size();
  while (end > start &&
         std::isspace(static_cast<unsigned char>(rValue[end - 1]))) {
    --end;
  }
  return rValue.substr(start, end - start);
}

std::vector<std::string> split(const std::string &rLine) {
  std::vector<std::string> parts;
  std::stringstream ss(rLine);
  std::string token;
  while (std::getline(ss, token, ';')) {
    parts.push_back(trim(token));
  }
  return parts;
}

int parseInt(const std::string &rValue, const std::string &rField) {
  try {
    size_t idx = 0;
    int result = std::stoi(rValue, &idx);
    if (idx != rValue.size()) {
      throw std::invalid_argument("Trailing data");
    }
    return result;
  } catch (const std::exception &) {
    throw std::runtime_error("Invalid integer for field: " + rField);
  }
}

MonsterCategory parseCategory(const std::string &rValue) {
  if (rValue == "NORMAL") {
    return MonsterCategory::NORMAL;
  }
  if (rValue == "MINIBOSS") {
    return MonsterCategory::MINIBOSS;
  }
  if (rValue == "BOSS") {
    return MonsterCategory::BOSS;
  }
  throw std::runtime_error("Invalid monster category: " + rValue);
}

int expectedActCount(MonsterCategory category) {
  switch (category) {
  case MonsterCategory::NORMAL:
    return 2;
  case MonsterCategory::MINIBOSS:
    return 3;
  case MonsterCategory::BOSS:
    return 4;
  }
  return 0;
}

std::vector<std::string> parseActIds(const std::vector<std::string> &rParts,
                                     MonsterCategory category,
                                     const ActCatalog &rCatalog) {
  if (rParts.size() != 10) {
    throw std::runtime_error("Malformed monsters line: expected 10 columns");
  }

  const int required = expectedActCount(category);
  std::vector<std::string> actIds;
  actIds.reserve(static_cast<size_t>(required));

  for (int i = 0; i < 4; ++i) {
    const std::string &rActId = rParts[static_cast<size_t>(6 + i)];
    if (i < required) {
      if (rActId.empty() || rActId == "-") {
        throw std::runtime_error("Missing ACT id for required slot");
      }
      if (!rCatalog.has(rActId)) {
        throw std::runtime_error("Unknown ACT id: " + rActId);
      }
      actIds.push_back(rActId);
      continue;
    }

    if (!rActId.empty() && rActId != "-") {
      throw std::runtime_error("Too many ACT ids for monster category");
    }
  }

  return actIds;
}
} // namespace

Inventory CsvLoader::loadItems(const std::string &rPath) const {
  std::ifstream file(rPath);
  if (!file) {
    throw std::runtime_error("Items file not found: " + rPath);
  }

  Inventory inventory;
  std::string line;
  size_t lineNumber = 0;
  while (std::getline(file, line)) {
    ++lineNumber;
    if (trim(line).empty()) {
      continue;
    }
    std::vector<std::string> parts = split(line);
    if (!parts.empty() && parts[0] == "nom") {
      continue;
    }
    if (parts.size() != 4) {
      throw std::runtime_error("Malformed items line at " +
                               std::to_string(lineNumber));
    }

    const std::string &rName = parts[0];
    const std::string &rType = parts[1];
    if (rName.empty()) {
      throw std::runtime_error("Empty item name at line " +
                               std::to_string(lineNumber));
    }

    int value = parseInt(parts[2], "valeur");
    int quantity = parseInt(parts[3], "quantite");
    if (value <= 0) {
      throw std::runtime_error("Invalid item value at line " +
                               std::to_string(lineNumber));
    }
    if (quantity < 0) {
      throw std::runtime_error("Invalid item quantity at line " +
                               std::to_string(lineNumber));
    }

    ItemType type = ItemType::HEAL;
    if (rType != "HEAL") {
      throw std::runtime_error("Invalid item type: " + rType);
    }

    Item item(rName, type, value);
    inventory.add(item, quantity);
  }

  return inventory;
}

std::vector<std::unique_ptr<Monster>>
CsvLoader::loadMonsters(const std::string &rPath,
                        const ActCatalog &rCatalog) const {
  std::ifstream file(rPath);
  if (!file) {
    throw std::runtime_error("Monsters file not found: " + rPath);
  }

  std::vector<std::unique_ptr<Monster>> monsters;
  std::string line;
  size_t lineNumber = 0;
  while (std::getline(file, line)) {
    ++lineNumber;
    if (trim(line).empty()) {
      continue;
    }
    std::vector<std::string> parts = split(line);
    if (!parts.empty() && parts[0] == "categorie") {
      continue;
    }
    if (parts.size() != 10) {
      throw std::runtime_error("Malformed monsters line at " +
                               std::to_string(lineNumber));
    }

    MonsterCategory category = parseCategory(parts[0]);
    const std::string &rName = parts[1];
    if (rName.empty()) {
      throw std::runtime_error("Empty monster name at line " +
                               std::to_string(lineNumber));
    }

    int hp = parseInt(parts[2], "hp");
    int atk = parseInt(parts[3], "atk");
    int def = parseInt(parts[4], "def");
    int mercyGoal = parseInt(parts[5], "mercyGoal");

    if (hp <= 0 || atk < 0 || def < 0 || mercyGoal <= 0) {
      throw std::runtime_error("Invalid monster stats at line " +
                               std::to_string(lineNumber));
    }

    std::vector<std::string> actIds;
    try {
      actIds = parseActIds(parts, category, rCatalog);
    } catch (const std::exception &rEx) {
      throw std::runtime_error("Invalid ACT data at line " +
                               std::to_string(lineNumber) + ": " + rEx.what());
    }

    if (category == MonsterCategory::NORMAL) {
      monsters.push_back(std::make_unique<NormalMonster>(rName, hp, atk, def,
                                                         mercyGoal, actIds));
    } else if (category == MonsterCategory::MINIBOSS) {
      monsters.push_back(std::make_unique<MiniBossMonster>(rName, hp, atk, def,
                                                           mercyGoal, actIds));
    } else {
      monsters.push_back(std::make_unique<BossMonster>(rName, hp, atk, def,
                                                       mercyGoal, actIds));
    }
  }

  return monsters;
}
