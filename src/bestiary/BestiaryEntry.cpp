#include "../../include/bestiary/BestiaryEntry.hpp"

BestiaryEntry::BestiaryEntry(const std::string &rMonsterName,
                             MonsterCategory category, int maxHp, int atk,
                             int def, CombatResult result)
    : mMonsterName(rMonsterName), mCategory(category), mMaxHp(maxHp), mAtk(atk),
      mDef(def), mResult(result) {}

const std::string &BestiaryEntry::getMonsterName() const {
  return mMonsterName;
}

MonsterCategory BestiaryEntry::getCategory() const { return mCategory; }

int BestiaryEntry::getMaxHp() const { return mMaxHp; }

int BestiaryEntry::getAtk() const { return mAtk; }

int BestiaryEntry::getDef() const { return mDef; }

CombatResult BestiaryEntry::getResult() const { return mResult; }
