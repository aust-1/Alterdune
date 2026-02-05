#pragma once

#include <string>

#include "../utils/Types.hpp"

class BestiaryEntry {
public:
  BestiaryEntry(const std::string &rMonsterName, MonsterCategory category,
                int maxHp, int atk, int def, CombatResult result);

  const std::string &getMonsterName() const;
  MonsterCategory getCategory() const;
  int getMaxHp() const;
  int getAtk() const;
  int getDef() const;
  CombatResult getResult() const;

private:
  std::string mMonsterName;
  MonsterCategory mCategory;
  int mMaxHp;
  int mAtk;
  int mDef;
  CombatResult mResult;
};
