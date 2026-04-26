#pragma once

#include <memory>

#include "Monster.hpp"

class MiniBossMonster : public Monster {
public:
  MiniBossMonster(const std::string &rName, int maxHp, int atk, int def,
                  int mercyGoal, const std::vector<std::string> &rActIds);
  int getAllowedActCount() const override;
  std::unique_ptr<Monster> clone() const override;
};
