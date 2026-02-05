#include "../../include/systems/Game.hpp"

#include <cstddef>
#include <iostream>
#include <limits>
#include <random>
#include <utility>

#include "../../include/systems/Combat.hpp"

namespace {
int readMenuChoice() {
  int choice = 0;
  std::cin >> choice;
  if (!std::cin) {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return -1;
  }
  return choice;
}
} // namespace

Game::Game(Player player) : mPlayer(std::move(player)) {}

void Game::run() { mainMenu(); }

void Game::mainMenu() {
  bool isRunning = true;
  while (isRunning) {
    if (mPlayer.getVictories() >= 10) {
      checkEndings();
      return;
    }

    std::cout << "\n=== ALTERDUNE ===\n";
    std::cout << "1) Bestiaire\n";
    std::cout << "2) Demarrer un combat\n";
    std::cout << "3) Statistiques du personnage\n";
    std::cout << "4) Items\n";
    std::cout << "5) Quitter\n";
    std::cout << "> ";

    int choice = readMenuChoice();
    switch (choice) {
    case 1:
      showBestiary();
      break;
    case 2:
      startCombat();
      break;
    case 3:
      showStats();
      break;
    case 4:
      showItems();
      break;
    case 5:
      isRunning = false;
      break;
    default:
      std::cout << "Choix invalide.\n";
      break;
    }
  }
}

void Game::startCombat() {
  if (mMonsters.empty()) {
    std::cout << "Aucun monstre disponible.\n";
    return;
  }

  static thread_local std::mt19937 sRng(std::random_device{}());
  std::uniform_int_distribution<std::size_t> dist(0, mMonsters.size() - 1);
  Monster& rMonster = *mMonsters[dist(sRng)];
  Combat combat(mPlayer, rMonster, mCatalog);
  CombatResult result = combat.start();
  if (!mPlayer.isAlive()) {
    std::cout << "Defaite.\n";
    return;
  }

  if (result == CombatResult::KILLED) {
    mPlayer.addKill();
    mPlayer.addVictory();
  } else {
    mPlayer.addSpare();
    mPlayer.addVictory();
  }

  BestiaryEntry entry(rMonster.getName(), rMonster.getCategory(),
                      rMonster.getMaxHp(), rMonster.getAtk(), rMonster.getDef(),
                      result);
  mBestiary.add(entry);
  std::cout << "Combat termine. Victoires: " << mPlayer.getVictories() << "\n";
}

void Game::showStats() const {
  std::cout << "\n=== Statistiques ===\n";
  std::cout << "Nom: " << mPlayer.getName() << "\n";
  std::cout << "HP: " << mPlayer.getHp() << "/" << mPlayer.getMaxHp() << "\n";
  std::cout << "Tues: " << mPlayer.getKills() << "\n";
  std::cout << "Epargnes: " << mPlayer.getSpares() << "\n";
  std::cout << "Victoires: " << mPlayer.getVictories() << "/10\n";
}

void Game::showItems() const {
  std::cout << "\n=== Items ===\n";
  const Inventory &rInventory = mPlayer.getInventory();
  const std::vector<Item> items = rInventory.listItems();
  if (items.empty()) {
    std::cout << "Inventaire vide.\n";
    return;
  }
  for (const Item &rItem : items) {
    std::cout << "- " << rItem.getName() << " x"
              << rInventory.getQuantity(rItem.getName()) << "\n";
  }
}

void Game::showBestiary() const {
  std::cout << "\n=== Bestiaire ===\n";
  const std::vector<BestiaryEntry> &rEntries = mBestiary.list();
  if (rEntries.empty()) {
    std::cout << "Aucun monstre vaincu.\n";
    return;
  }
  for (const BestiaryEntry &rEntry : rEntries) {
    std::cout << "- " << rEntry.getMonsterName() << " | HP "
              << rEntry.getMaxHp() << " | ATK " << rEntry.getAtk() << " | DEF "
              << rEntry.getDef() << " | Resultat "
              << (rEntry.getResult() == CombatResult::KILLED ? "Tue"
                                                             : "Epargne")
              << "\n";
  }
}

void Game::checkEndings() const {
  std::cout << "\n=== Fin de partie ===\n";
  if (mPlayer.getKills() == 0 && mPlayer.getSpares() > 0) {
    std::cout << "Fin Pacifiste\n";
    return;
  }
  if (mPlayer.getSpares() == 0 && mPlayer.getKills() > 0) {
    std::cout << "Fin Genocidaire\n";
    return;
  }
  std::cout << "Fin Neutre\n";
}

Player &Game::getPlayer() { return mPlayer; }

const Player &Game::getPlayer() const { return mPlayer; }

Bestiary &Game::getBestiary() { return mBestiary; }

const Bestiary &Game::getBestiary() const { return mBestiary; }

ActCatalog &Game::getCatalog() { return mCatalog; }

const ActCatalog &Game::getCatalog() const { return mCatalog; }

std::vector<std::unique_ptr<Monster>> &Game::getMonsters() { return mMonsters; }

const std::vector<std::unique_ptr<Monster>> &Game::getMonsters() const {
  return mMonsters;
}
