#pragma once

#include <memory>

#include "Monster.hpp"

class NormalMonster : public Monster {
public:
  NormalMonster(const std::string &rName, int maxHp, int atk, int def,
                int mercyGoal, const std::vector<std::string> &rActIds);
  int getAllowedActCount() const override;
  std::unique_ptr<Monster> clone() const override;
};
