#include "../../include/systems/Game.hpp"

#include <cstddef>
#include <random>
#include <string>
#include <utility>
#include <vector>

#include "../../include/systems/Combat.hpp"

namespace {
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

const char *resultToString(CombatResult result) {
  switch (result) {
  case CombatResult::KILLED:
    return "Élimine";
  case CombatResult::SPARED:
    return "Épargne";
  case CombatResult::PLAYER_DEFEATED:
    return "Défaite";
  }
  return "Inconnu";
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
  mUi.printBanner("ALTERDUNE", "A tactical tale of fights and mercy");
  mUi.printSection("Session initialisée");
  mUi.printStat("Nom", mPlayer.getName());
  mUi.printProgressBar("HP", mPlayer.getHp(), mPlayer.getMaxHp());
  mUi.printProgressBar("Victoires", mPlayer.getVictories(), 10);

  const Inventory &rInventory = mPlayer.getInventory();
  const std::vector<Item> items = rInventory.listItems();
  mUi.printSection("Inventaire de depart");
  if (items.empty()) {
    mUi.printWarning("Aucun item en reserve.");
    return;
  }

  std::vector<std::vector<std::string>> rows;
  rows.reserve(items.size());
  for (const Item &rItem : items) {
    rows.push_back({rItem.getName(),
                    std::to_string(rInventory.getQuantity(rItem.getName())),
                    itemDescription(rItem)});
  }
  mUi.printTable({"Item", "Quantité", "Effet"}, rows);
}

void Game::mainMenu() {
  bool isRunning = true;
  while (isRunning && !mIsGameOver) {
    if (mPlayer.getVictories() >= 10) {
      checkEndings();
      return;
    }

    mUi.printBanner("ALTERDUNE", "Choisissez votre prochaine action");
    mUi.printProgressBar("HP", mPlayer.getHp(), mPlayer.getMaxHp());
    mUi.printProgressBar("Victoires", mPlayer.getVictories(), 10);
    mUi.printMenu({"Bestiaire", "Démarrer un combat", "Statistiques", "Items",
                   "Quitter"});
    mUi.printPrompt("Votre choix [1-5] > ");

    int choice = mUi.readMenuChoice();
    if (choice == TerminalUI::kInputClosed) {
      mUi.printWarning("Flux d'entrée fermé. Fermeture du jeu.");
      return;
    }

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
      mUi.printError("Choix invalide. Entrez un nombre de 1 a 5.");
      break;
    }
  }
}

bool Game::startCombat() {
  if (mMonsters.empty()) {
    mUi.printWarning("Aucun monstre disponible.");
    return true;
  }

  static thread_local std::mt19937 sRng(std::random_device{}());
  std::uniform_int_distribution<std::size_t> dist(0, mMonsters.size() - 1);
  std::unique_ptr<Monster> monster = mMonsters[dist(sRng)]->clone();
  Monster &rMonster = *monster;

  Combat combat(mPlayer, rMonster, mCatalog, mUi);
  CombatResult result = combat.start();

  if (result == CombatResult::PLAYER_DEFEATED || !mPlayer.isAlive()) {
    mIsGameOver = true;
    mUi.printError("Vous avez perdu le combat.");
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
  mUi.printSuccess("Combat terminé: " + std::string(resultToString(result)) +
                   ". Victoires: " + std::to_string(mPlayer.getVictories()) +
                   "/10");
  return true;
}

void Game::showStats() const {
  mUi.printBanner("STATISTIQUES", mPlayer.getName());
  mUi.printProgressBar("HP", mPlayer.getHp(), mPlayer.getMaxHp());
  mUi.printProgressBar("Victoires", mPlayer.getVictories(), 10);
  mUi.printStat("Monstres tués", std::to_string(mPlayer.getKills()));
  mUi.printStat("Monstres épargnés", std::to_string(mPlayer.getSpares()));
}

void Game::showItems() {
  while (true) {
    mUi.printBanner("INVENTAIRE", "Utilisez un item hors combat");
    Inventory &rInventory = mPlayer.getInventory();
    const std::vector<Item> items = rInventory.listItems();
    if (items.empty()) {
      mUi.printWarning("Inventaire vide.");
      return;
    }

    std::vector<std::vector<std::string>> rows;
    rows.reserve(items.size());
    for (size_t i = 0; i < items.size(); ++i) {
      const Item &rItem = items[i];
      rows.push_back({std::to_string(i + 1), rItem.getName(),
                      std::to_string(rInventory.getQuantity(rItem.getName())),
                      itemDescription(rItem)});
    }
    mUi.printTable({"#", "Item", "Quantité", "Effet"}, rows);
    mUi.printPrompt("Votre choix [1-" + std::to_string(items.size()) +
                    ", 0 retour] > ");

    int choice = mUi.readMenuChoice();
    if (choice == TerminalUI::kInputClosed) {
      return;
    }

    if (choice == 0) {
      return;
    }
    if (choice < 1 || choice > static_cast<int>(items.size())) {
      mUi.printError("Choix invalide.");
      continue;
    }

    const Item &rItem = items[static_cast<size_t>(choice - 1)];
    if (!rInventory.use(rItem.getName(), mPlayer)) {
      mUi.printWarning("Item indisponible.");
      continue;
    }
    mUi.printSuccess(rItem.getName() + " utilisé.");
    mUi.printProgressBar("HP", mPlayer.getHp(), mPlayer.getMaxHp());
  }
}

void Game::showBestiary() const {
  mUi.printBanner("BESTIAIRE", "Historique des rencontres");
  const std::vector<BestiaryEntry> &rEntries = mBestiary.list();
  if (rEntries.empty()) {
    mUi.printInfo("Aucune entrée pour le moment.");
    return;
  }

  std::vector<std::vector<std::string>> rows;
  rows.reserve(rEntries.size());
  for (const BestiaryEntry &rEntry : rEntries) {
    rows.push_back(
        {rEntry.getMonsterName(), categoryToString(rEntry.getCategory()),
         std::to_string(rEntry.getMaxHp()), std::to_string(rEntry.getAtk()),
         std::to_string(rEntry.getDef()), resultToString(rEntry.getResult())});
  }
  mUi.printTable({"Nom", "Categorie", "HP", "ATK", "DEF", "Résultat"}, rows);
}

void Game::checkEndings() const {
  mUi.printBanner("FIN DE PARTIE", "Votre trajectoire est scellée");
  if (mPlayer.getKills() == 0 && mPlayer.getSpares() > 0) {
    mUi.printSuccess("Fin Pacifiste");
    return;
  }
  if (mPlayer.getSpares() == 0 && mPlayer.getKills() > 0) {
    mUi.printWarning("Fin Genocidaire");
    return;
  }
  mUi.printInfo("Fin Neutre");
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
