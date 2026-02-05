#pragma once

#include <string>
#include <vector>

#include "../utils/Types.hpp"
#include "Entity.hpp"

class Monster : public Entity {
public:
  Monster(const std::string &rName, int maxHp, int atk, int def,
          MonsterCategory category, int mercyGoal,
          const std::vector<std::string> &rActIds);

  MonsterCategory getCategory() const;
  int getMercy() const;
  int getMercyGoal() const;
  const std::vector<std::string> &getActIds() const;

  void setMercy(int value);

private:
  MonsterCategory mCategory;
  int mMercy;
  int mMercyGoal;
  std::vector<std::string> mActIds;
};
