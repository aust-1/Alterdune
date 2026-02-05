#include "../../include/entities/Monster.hpp"

Monster::Monster(const std::string &rName, int maxHp, int atk, int def,
                 MonsterCategory category, int mercyGoal,
                 const std::vector<std::string> &rActIds)
    : Entity(rName, maxHp, atk, def), mCategory(category), mMercy(0),
      mMercyGoal(mercyGoal), mActIds(rActIds) {}

MonsterCategory Monster::getCategory() const { return mCategory; }

int Monster::getMercy() const { return mMercy; }

int Monster::getMercyGoal() const { return mMercyGoal; }

const std::vector<std::string> &Monster::getActIds() const { return mActIds; }

void Monster::setMercy(int value) {
  if (value < 0) {
    mMercy = 0;
    return;
  }
  if (value > mMercyGoal) {
    mMercy = mMercyGoal;
    return;
  }
  mMercy = value;
}
