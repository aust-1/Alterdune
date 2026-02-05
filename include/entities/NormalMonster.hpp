#pragma once

#include "Monster.hpp"

class NormalMonster : public Monster {
public:
  NormalMonster(const std::string &rName, int maxHp, int atk, int def,
                int mercyGoal, const std::vector<std::string> &rActIds);
};
