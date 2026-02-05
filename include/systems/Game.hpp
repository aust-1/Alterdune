#pragma once

#include <memory>
#include <vector>

#include "../acts/ActCatalog.hpp"
#include "../bestiary/Bestiary.hpp"
#include "../entities/Monster.hpp"
#include "../entities/Player.hpp"
#include "../utils/Types.hpp"

class Game {
public:
  Game(Player player);

  void run();
  void mainMenu();
  void startCombat();
  void showStats() const;
  void showItems() const;
  void showBestiary() const;
  void checkEndings() const;

  Player &getPlayer();
  const Player &getPlayer() const;
  Bestiary &getBestiary();
  const Bestiary &getBestiary() const;
  ActCatalog &getCatalog();
  const ActCatalog &getCatalog() const;
  std::vector<std::unique_ptr<Monster>> &getMonsters();
  const std::vector<std::unique_ptr<Monster>> &getMonsters() const;

private:
  Player mPlayer;
  Bestiary mBestiary;
  ActCatalog mCatalog;
  std::vector<std::unique_ptr<Monster>> mMonsters;
};
