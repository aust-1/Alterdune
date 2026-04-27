#pragma once

#include <string>

#include "../items/Inventory.hpp"
#include "Entity.hpp"

class Player : public Entity {
public:
  Player(const std::string &rName, int maxHp, int atk, int def);

  int getKills() const;
  int getSpares() const;
  int getVictories() const;

  void addKill();
  void addSpare();
  void addVictory();
  void heal(int amount);

  Inventory &getInventory();
  const Inventory &getInventory() const;

private:
  int mKills;
  int mSpares;
  int mVictories;
  Inventory mInventory;
};
