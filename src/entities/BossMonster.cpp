#include "../../include/entities/BossMonster.hpp"

BossMonster::BossMonster(const std::string &rName, int maxHp, int atk, int def,
                         int mercyGoal, const std::vector<std::string> &rActIds)
    : Monster(rName, maxHp, atk, def, MonsterCategory::BOSS, mercyGoal,
              rActIds) {}

int BossMonster::getAllowedActCount() const { return 4; }

std::unique_ptr<Monster> BossMonster::clone() const {
  return std::make_unique<BossMonster>(*this);
}
