#include "../../include/entities/Entity.hpp"

Entity::Entity(const std::string &rName, int maxHp, int atk, int def)
    : mName(rName), mHp(maxHp), mMaxHp(maxHp), mAtk(atk), mDef(def) {}

const std::string &Entity::getName() const { return mName; }

int Entity::getHp() const { return mHp; }

int Entity::getMaxHp() const { return mMaxHp; }

int Entity::getAtk() const { return mAtk; }

int Entity::getDef() const { return mDef; }

void Entity::setHp(int value) {
  if (value < 0) {
    mHp = 0;
    return;
  }
  if (value > mMaxHp) {
    mHp = mMaxHp;
    return;
  }
  mHp = value;
}

bool Entity::isAlive() const { return mHp > 0; }

void Entity::takeDamage(int amount) {
  if (amount <= 0) {
    return;
  }
  setHp(mHp - amount);
}
