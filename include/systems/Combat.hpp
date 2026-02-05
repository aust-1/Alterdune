#pragma once

#include <string>

#include "../acts/ActCatalog.hpp"
#include "../entities/Monster.hpp"
#include "../entities/Player.hpp"
#include "../utils/RandomService.hpp"
#include "../utils/Types.hpp"

class Combat {
public:
  Combat(Player &rPlayer, Monster &rMonster, const ActCatalog &rCatalog);

  CombatResult start();
  void playerFight();
  void playerAct(const std::string &rActId);
  void playerItem(const std::string &rItemName);
  bool playerMercy();
  void monsterTurn();

private:
  Player &mRPlayer;
  Monster &mRMonster;
  const ActCatalog &mRCatalog;
  RandomService mRandomService;
};
