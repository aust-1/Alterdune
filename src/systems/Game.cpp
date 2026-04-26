#include "../../include/systems/Game.hpp"

#include <cstddef>
#include <iostream>
#include <limits>
#include <random>
#include <string>
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

const char *categoryToString(MonsterCategory category) {
  switch (category) {
  case MonsterCategory::NORMAL:
    return "NORMAL";
  case MonsterCategory::MINIBOSS:
    return "MINIBOSS";
  case MonsterCategory::BOSS:
    return "BOSS";
  }
  return "INCONNUE";
}

std::string itemDescription(const Item &rItem) {
  if (rItem.getType() == ItemType::HEAL) {
    return "soigne " + std::to_string(rItem.getValue()) + " HP";
  }
  return "effet inconnu";
}
} // namespace

Game::Game(Player player) : mPlayer(std::move(player)), mIsGameOver(false) {}

void Game::run() { mainMenu(); }

void Game::showStartupSummary() const {
  std::cout << "\n=== Partie initialisée ===\n";
  std::cout << "Nom: " << mPlayer.getName() << "\n";
  std::cout << "HP: " << mPlayer.getHp() << "/" << mPlayer.getMaxHp() << "\n";
  std::cout << "Items:\n";

  const Inventory &rInventory = mPlayer.getInventory();
  const std::vector<Item> items = rInventory.listItems();
  if (items.empty()) {
    std::cout << "- Aucun item\n";
    return;
  }

  for (const Item &rItem : items) {
    std::cout << "- " << rItem.getName() << " x"
              << rInventory.getQuantity(rItem.getName()) << " ("
              << itemDescription(rItem) << ")\n";
  }
}

void Game::mainMenu() {
  bool isRunning = true;
  while (isRunning && !mIsGameOver) {
    if (mPlayer.getVictories() >= 10) {
      checkEndings();
      return;
    }

    std::cout << "\n=== ALTERDUNE ===\n";
    std::cout << "1) Bestiaire\n";
    std::cout << "2) Démarrer un combat\n";
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
      if (!startCombat()) {
        isRunning = false;
      }
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

bool Game::startCombat() {
  if (mMonsters.empty()) {
    std::cout << "Aucun monstre disponible.\n";
    return true;
  }

  static thread_local std::mt19937 sRng(std::random_device{}());
  std::uniform_int_distribution<std::size_t> dist(0, mMonsters.size() - 1);
  std::unique_ptr<Monster> monster = mMonsters[dist(sRng)]->clone();
  Monster &rMonster = *monster;

  Combat combat(mPlayer, rMonster, mCatalog);
  CombatResult result = combat.start();

  if (result == CombatResult::PLAYER_DEFEATED || !mPlayer.isAlive()) {
    mIsGameOver = true;
    std::cout << "Défaite.\n";
    return false;
  }

  if (result == CombatResult::KILLED) {
    mPlayer.addKill();
  } else if (result == CombatResult::SPARED) {
    mPlayer.addSpare();
  }
  mPlayer.addVictory();

  BestiaryEntry entry(rMonster.getName(), rMonster.getCategory(),
                      rMonster.getMaxHp(), rMonster.getAtk(), rMonster.getDef(),
                      result);
  mBestiary.add(entry);
  std::cout << "Combat terminé. Victoires: " << mPlayer.getVictories() << "\n";
  return true;
}

void Game::showStats() const {
  std::cout << "\n=== Statistiques ===\n";
  std::cout << "Nom: " << mPlayer.getName() << "\n";
  std::cout << "HP: " << mPlayer.getHp() << "/" << mPlayer.getMaxHp() << "\n";
  std::cout << "Tués: " << mPlayer.getKills() << "\n";
  std::cout << "Epargnés: " << mPlayer.getSpares() << "\n";
  std::cout << "Victoires: " << mPlayer.getVictories() << "/10\n";
}

void Game::showItems() {
  while (true) {
    std::cout << "\n=== Items ===\n";
    Inventory &rInventory = mPlayer.getInventory();
    const std::vector<Item> items = rInventory.listItems();
    if (items.empty()) {
      std::cout << "Inventaire vide.\n";
      return;
    }

    for (size_t i = 0; i < items.size(); ++i) {
      const Item &rItem = items[i];
      std::cout << (i + 1) << ") " << rItem.getName() << " x"
                << rInventory.getQuantity(rItem.getName()) << " ("
                << itemDescription(rItem) << ")\n";
    }
    std::cout << "0) Retour\n";
    std::cout << "> ";

    int choice = readMenuChoice();
    if (choice == 0) {
      return;
    }
    if (choice < 1 || choice > static_cast<int>(items.size())) {
      std::cout << "Choix invalide.\n";
      continue;
    }

    const Item &rItem = items[static_cast<size_t>(choice - 1)];
    if (!rInventory.use(rItem.getName(), mPlayer)) {
      std::cout << "Item indisponible.\n";
      continue;
    }
    std::cout << rItem.getName() << " utilise. HP: " << mPlayer.getHp() << "/"
              << mPlayer.getMaxHp() << "\n";
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
    std::cout << "- " << rEntry.getMonsterName() << " | "
              << categoryToString(rEntry.getCategory()) << " | HP "
              << rEntry.getMaxHp() << " | ATK " << rEntry.getAtk() << " | DEF "
              << rEntry.getDef() << " | Résultat "
              << (rEntry.getResult() == CombatResult::KILLED ? "Tué"
                                                             : "Epargné")
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
