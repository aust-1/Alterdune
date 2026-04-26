#include "../../include/entities/MiniBossMonster.hpp"

MiniBossMonster::MiniBossMonster(const std::string &rName, int maxHp, int atk,
                                 int def, int mercyGoal,
                                 const std::vector<std::string> &rActIds)
    : Monster(rName, maxHp, atk, def, MonsterCategory::MINIBOSS, mercyGoal,
              rActIds) {}

int MiniBossMonster::getAllowedActCount() const { return 3; }

std::unique_ptr<Monster> MiniBossMonster::clone() const {
  return std::make_unique<MiniBossMonster>(*this);
}
