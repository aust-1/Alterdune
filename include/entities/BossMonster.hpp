#pragma once

#include "Monster.hpp"

class BossMonster : public Monster {
public:
  BossMonster(const std::string &rName, int maxHp, int atk, int def,
              int mercyGoal, const std::vector<std::string> &rActIds);
};
