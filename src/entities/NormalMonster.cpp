#include "../../include/entities/NormalMonster.hpp"

NormalMonster::NormalMonster(const std::string &rName, int maxHp, int atk,
                             int def, int mercyGoal,
                             const std::vector<std::string> &rActIds)
    : Monster(rName, maxHp, atk, def, MonsterCategory::NORMAL, mercyGoal,
              rActIds) {}
