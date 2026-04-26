#include "../../include/systems/Combat.hpp"

#include <algorithm>
#include <cstddef>
#include <iostream>
#include <limits>

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

Combat::Combat(Player &rPlayer, Monster &rMonster, const ActCatalog &rCatalog)
    : mRPlayer(rPlayer), mRMonster(rMonster), mRCatalog(rCatalog) {}

CombatResult Combat::start() {
  std::cout << "\n=== Combat ===\n";
  while (mRPlayer.isAlive() && mRMonster.isAlive()) {
    std::cout << "\n"
              << mRPlayer.getName() << " HP " << mRPlayer.getHp() << "/"
              << mRPlayer.getMaxHp() << "\n";
    std::cout << mRMonster.getName() << " HP " << mRMonster.getHp() << "/"
              << mRMonster.getMaxHp() << " | Mercy " << mRMonster.getMercy()
              << "/" << mRMonster.getMercyGoal() << "\n";
    std::cout << "1) FIGHT  2) ACT  3) ITEM  4) MERCY\n";
    std::cout << "> ";

    int choice = readMenuChoice();
    switch (choice) {
    case 1:
      playerFight();
      break;
    case 2: {
      const std::vector<std::string> &rActIds = mRMonster.getActIds();
      if (rActIds.empty()) {
        std::cout << "Aucune action ACT disponible.\n";
        break;
      }
      std::cout << "Choisir une action:\n";
      int index = 1;
      std::vector<std::string> actList;
      const size_t maxActs = std::min(
          rActIds.size(), static_cast<size_t>(mRMonster.getAllowedActCount()));
      for (size_t i = 0; i < maxActs; ++i) {
        const std::string &rActId = rActIds[i];
        if (!mRCatalog.has(rActId)) {
          continue;
        }
        std::cout << index << ") " << rActId << "\n";
        actList.push_back(rActId);
        ++index;
      }
      if (actList.empty()) {
        std::cout << "Aucune action ACT valide.\n";
        break;
      }
      std::cout << "> ";
      int actChoice = readMenuChoice();
      if (actChoice < 1 || actChoice > static_cast<int>(actList.size())) {
        std::cout << "Choix invalide.\n";
        break;
      }
      playerAct(actList[static_cast<size_t>(actChoice - 1)]);
      break;
    }
    case 3: {
      const Inventory &rInventory = mRPlayer.getInventory();
      const std::vector<Item> items = rInventory.listItems();
      if (items.empty()) {
        std::cout << "Inventaire vide.\n";
        break;
      }
      std::cout << "Choisir un item:\n";
      for (size_t i = 0; i < items.size(); ++i) {
        const Item &rItem = items[i];
        std::cout << (i + 1) << ") " << rItem.getName() << " x"
                  << rInventory.getQuantity(rItem.getName()) << "\n";
      }
      std::cout << "> ";
      int itemChoice = readMenuChoice();
      if (itemChoice < 1 || itemChoice > static_cast<int>(items.size())) {
        std::cout << "Choix invalide.\n";
        break;
      }
      playerItem(items[static_cast<size_t>(itemChoice - 1)].getName());
      break;
    }
    case 4:
      if (playerMercy()) {
        std::cout << "Vous épargnez le monstre.\n";
        return CombatResult::SPARED;
      }
      std::cout << "Mercy insuffisante.\n";
      break;
    default:
      std::cout << "Choix invalide.\n";
      break;
    }

    if (mRMonster.isAlive()) {
      monsterTurn();
    }
  }

  if (!mRPlayer.isAlive()) {
    std::cout << "Vous avez été vaincu.\n";
    return CombatResult::PLAYER_DEFEATED;
  }
  if (!mRMonster.isAlive()) {
    std::cout << "Vous avez vaincu le monstre.\n";
    return CombatResult::KILLED;
  }
  return CombatResult::SPARED;
}

void Combat::playerFight() {
  int damage = mRandomService.rollDamage(mRMonster.getMaxHp());
  mRMonster.takeDamage(damage);
  std::cout << "Dégâts infligés: " << damage << "\n";
}

void Combat::playerAct(const std::string &rActId) {
  if (!mRCatalog.has(rActId)) {
    std::cout << "Action ACT inconnue.\n";
    return;
  }
  const ActAction &action = mRCatalog.get(rActId);
  mRMonster.setMercy(mRMonster.getMercy() + action.getMercyDelta());
  std::cout << action.getText() << "\n";
  std::cout << "Mercy: " << mRMonster.getMercy() << "/"
            << mRMonster.getMercyGoal() << "\n";
}

void Combat::playerItem(const std::string &rItemName) {
  bool used = mRPlayer.getInventory().use(rItemName, mRPlayer);
  if (!used) {
    std::cout << "Item indisponible.\n";
  }
}

bool Combat::playerMercy() {
  if (mRMonster.getMercy() < mRMonster.getMercyGoal()) {
    return false;
  }
  return true;
}

void Combat::monsterTurn() {
  if (!mRMonster.isAlive()) {
    return;
  }
  int damage = mRandomService.rollDamage(mRPlayer.getMaxHp());
  mRPlayer.takeDamage(damage);
  std::cout << mRMonster.getName() << " attaque. Dégâts: " << damage << "\n";
}
