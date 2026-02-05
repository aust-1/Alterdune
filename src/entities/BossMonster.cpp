#include "../../include/entities/BossMonster.hpp"

BossMonster::BossMonster(const std::string &rName, int maxHp, int atk, int def,
                         int mercyGoal, const std::vector<std::string> &rActIds)
    : Monster(rName, maxHp, atk, def, MonsterCategory::BOSS, mercyGoal,
              rActIds) {}
