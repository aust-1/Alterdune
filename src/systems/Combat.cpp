#include "../../include/systems/Combat.hpp"

#include <algorithm>
#include <cstddef>
#include <string>
#include <vector>

namespace {
std::string itemDescription(const Item &rItem) {
  if (rItem.getType() == ItemType::HEAL) {
    return "Heal " + std::to_string(rItem.getValue()) + " HP";
  }
  return "Unknown effect";
}
} // namespace

Combat::Combat(Player &rPlayer, Monster &rMonster, const ActCatalog &rCatalog,
               TerminalUI &rUi)
    : mRPlayer(rPlayer), mRMonster(rMonster), mRCatalog(rCatalog), mRUi(rUi) {}

CombatResult Combat::start() {
  mEventLog.clear();
  pushEvent("Le combat commence contre " + mRMonster.getName() + ".");

  while (mRPlayer.isAlive() && mRMonster.isAlive()) {
    mRUi.printBanner("COMBAT", "Affrontement contre " + mRMonster.getName());
    mRUi.printProgressBar(mRPlayer.getName() + " HP", mRPlayer.getHp(),
                          mRPlayer.getMaxHp());
    mRUi.printProgressBar(mRMonster.getName() + " HP", mRMonster.getHp(),
                          mRMonster.getMaxHp());
    mRUi.printProgressBar("Mercy", mRMonster.getMercy(),
                          mRMonster.getMercyGoal());

    if (!mEventLog.empty()) {
      mRUi.printSection("Journal");
      const size_t first = mEventLog.size() > 4 ? mEventLog.size() - 4 : 0;
      for (size_t i = first; i < mEventLog.size(); ++i) {
        mRUi.printInfo(mEventLog[i]);
      }
    }

    mRUi.printSection("Actions");
    mRUi.printMenu({"FIGHT", "ACT", "ITEM", "MERCY"});
    mRUi.printPrompt("Votre choix [1-4] > ");

    const int choice = mRUi.readMenuChoice();
    if (choice == TerminalUI::kInputClosed) {
      mRUi.printWarning("Flux d'entrée fermé pendant le combat.");
      return CombatResult::PLAYER_DEFEATED;
    }

    bool playerSpentTurn = false;

    switch (choice) {
    case 1:
      playerFight();
      playerSpentTurn = true;
      break;
    case 2: {
      const std::vector<std::string> &rActIds = mRMonster.getActIds();
      if (rActIds.empty()) {
        mRUi.printWarning("Aucune action ACT disponible.");
        break;
      }

      std::vector<std::string> actList;
      std::vector<std::string> actMenu;
      const size_t maxActs = std::min(
          rActIds.size(), static_cast<size_t>(mRMonster.getAllowedActCount()));

      for (size_t i = 0; i < maxActs; ++i) {
        const std::string &rActId = rActIds[i];
        if (!mRCatalog.has(rActId)) {
          continue;
        }

        const ActAction &action = mRCatalog.get(rActId);
        actMenu.push_back(rActId + " (Mercy " +
                          (action.getMercyDelta() >= 0 ? "+" : "") +
                          std::to_string(action.getMercyDelta()) + ")");
        actList.push_back(rActId);
      }

      if (actList.empty()) {
        mRUi.printWarning("Aucune action ACT valide.");
        break;
      }

      mRUi.printSection("Choisir une action ACT");
      mRUi.printMenu(actMenu);
      mRUi.printPrompt("Votre choix [1-" + std::to_string(actList.size()) +
                       "] > ");

      int actChoice = mRUi.readMenuChoice();
      if (actChoice == TerminalUI::kInputClosed) {
        mRUi.printWarning("Flux d'entrée fermé pendant le combat.");
        return CombatResult::PLAYER_DEFEATED;
      }

      if (actChoice < 1 || actChoice > static_cast<int>(actList.size())) {
        mRUi.printError("Choix invalide.");
        break;
      }

      playerAct(actList[static_cast<size_t>(actChoice - 1)]);
      playerSpentTurn = true;
      break;
    }
    case 3: {
      const Inventory &rInventory = mRPlayer.getInventory();
      const std::vector<Item> items = rInventory.listItems();
      if (items.empty()) {
        mRUi.printWarning("Inventaire vide.");
        break;
      }

      std::vector<std::vector<std::string>> rows;
      rows.reserve(items.size());
      for (size_t i = 0; i < items.size(); ++i) {
        const Item &rItem = items[i];
        rows.push_back({std::to_string(i + 1), rItem.getName(),
                        std::to_string(rInventory.getQuantity(rItem.getName())),
                        itemDescription(rItem)});
      }

      mRUi.printSection("Choisir un item");
      mRUi.printTable({"#", "Item", "Quantité", "Effet"}, rows);
      mRUi.printPrompt("Votre choix [1-" + std::to_string(items.size()) +
                       "] > ");

      int itemChoice = mRUi.readMenuChoice();
      if (itemChoice == TerminalUI::kInputClosed) {
        mRUi.printWarning("Flux d'entrée fermé pendant le combat.");
        return CombatResult::PLAYER_DEFEATED;
      }

      if (itemChoice < 1 || itemChoice > static_cast<int>(items.size())) {
        mRUi.printError("Choix invalide.");
        break;
      }

      playerItem(items[static_cast<size_t>(itemChoice - 1)].getName());
      playerSpentTurn = true;
      break;
    }
    case 4:
      if (playerMercy()) {
        mRUi.printSuccess("Vous épargnez " + mRMonster.getName() + ".");
        pushEvent("Vous épargnez " + mRMonster.getName() + ".");
        return CombatResult::SPARED;
      }
      mRUi.printWarning(
          "Mercy insuffisante: " + std::to_string(mRMonster.getMercy()) + "/" +
          std::to_string(mRMonster.getMercyGoal()) + ".");
      pushEvent("Vous tentez MERCY sans succès.");
      playerSpentTurn = true;
      break;
    default:
      mRUi.printError("Choix invalide. Entrez un nombre de 1 à 4.");
      break;
    }

    if (playerSpentTurn && mRMonster.isAlive()) {
      monsterTurn();
    }
  }

  if (!mRPlayer.isAlive()) {
    mRUi.printError("Vous avez été vaincu.");
    return CombatResult::PLAYER_DEFEATED;
  }
  if (!mRMonster.isAlive()) {
    mRUi.printSuccess("Vous avez vaincu " + mRMonster.getName() + ".");
    return CombatResult::KILLED;
  }

  return CombatResult::SPARED;
}

void Combat::playerFight() {
  int damage = mRandomService.rollDamage(mRPlayer.getAtk(), mRMonster.getDef());
  mRMonster.takeDamage(damage);
  pushEvent("Vous frappez et infligez " + std::to_string(damage) + " dégâts.");
}

void Combat::playerAct(const std::string &rActId) {
  if (!mRCatalog.has(rActId)) {
    mRUi.printError("Action ACT inconnue.");
    return;
  }

  const ActAction &action = mRCatalog.get(rActId);
  mRMonster.setMercy(mRMonster.getMercy() + action.getMercyDelta());
  pushEvent(action.getText());
  pushEvent("Mercy: " + std::to_string(mRMonster.getMercy()) + "/" +
            std::to_string(mRMonster.getMercyGoal()));
}

void Combat::playerItem(const std::string &rItemName) {
  const int hpBefore = mRPlayer.getHp();
  bool used = mRPlayer.getInventory().use(rItemName, mRPlayer);
  if (!used) {
    mRUi.printWarning("Item indisponible.");
    return;
  }

  const int healed = std::max(0, mRPlayer.getHp() - hpBefore);
  pushEvent("Vous utilisez " + rItemName + " (" + std::to_string(healed) +
            " HP récupérés).");
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

  int damage = mRandomService.rollDamage(mRMonster.getAtk(), mRPlayer.getDef());
  mRPlayer.takeDamage(damage);
  pushEvent(mRMonster.getName() + " attaque et inflige " +
            std::to_string(damage) + " dégâts.");
}

void Combat::pushEvent(const std::string &rText) {
  mEventLog.push_back(rText);
  if (mEventLog.size() > 12) {
    mEventLog.erase(mEventLog.begin());
  }
}
