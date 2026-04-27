#pragma once

#include <string>

class Entity {
public:
  Entity(const std::string &rName, int maxHp, int atk, int def);
  virtual ~Entity() = default;

  const std::string &getName() const;
  int getHp() const;
  int getMaxHp() const;
  int getAtk() const;
  int getDef() const;

  bool isAlive() const;
  void takeDamage(int amount);

protected:
  void setHp(int value);

private:
  std::string mName;
  int mHp;
  int mMaxHp;
  int mAtk;
  int mDef;
};
