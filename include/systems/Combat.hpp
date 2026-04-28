#pragma once

#include <string>
#include <vector>

#include "../acts/ActCatalog.hpp"
#include "../entities/Monster.hpp"
#include "../entities/Player.hpp"
#include "../ui/TerminalUI.hpp"
#include "../utils/RandomService.hpp"
#include "../utils/Types.hpp"

class Combat {
public:
  Combat(Player &rPlayer, Monster &rMonster, const ActCatalog &rCatalog,
         TerminalUI &rUi);

  CombatResult start();
  void playerFight();
  void playerAct(const std::string &rActId);
  void playerItem(const std::string &rItemName);
  bool playerMercy();
  void monsterTurn();

private:
  void pushEvent(const std::string &rText);

  Player &mRPlayer;
  Monster &mRMonster;
  const ActCatalog &mRCatalog;
  TerminalUI &mRUi;
  RandomService mRandomService;
  std::vector<std::string> mEventLog;
};
