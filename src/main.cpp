#include "../include/io/CsvLoader.hpp"
#include "../include/systems/Game.hpp"

#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#endif

namespace {
bool isFileNotFoundError(const std::string &rMessage) {
  return rMessage.find("file not found") != std::string::npos;
}

std::string readPlayerName() {
  std::cout << "Entrez le nom de votre personnage: ";
  std::string name;
  std::getline(std::cin, name);
  if (name.empty()) {
    return "Héro";
  }
  return name;
}

Inventory loadItemsWithFallback(const CsvLoader &rLoader) {
  const std::vector<std::string> candidates = {"data/items.csv",
                                               "../data/items.csv"};
  std::string lastError;
  for (const std::string &rPath : candidates) {
    try {
      return rLoader.loadItems(rPath);
    } catch (const std::exception &rEx) {
      lastError = rEx.what();
      if (!isFileNotFoundError(lastError)) {
        throw;
      }
    }
  }
  throw std::runtime_error(lastError);
}

std::vector<std::unique_ptr<Monster>>
loadMonstersWithFallback(const CsvLoader &rLoader, const ActCatalog &rCatalog) {
  const std::vector<std::string> candidates = {"data/monsters.csv",
                                               "../data/monsters.csv"};
  std::string lastError;
  for (const std::string &rPath : candidates) {
    try {
      return rLoader.loadMonsters(rPath, rCatalog);
    } catch (const std::exception &rEx) {
      lastError = rEx.what();
      if (!isFileNotFoundError(lastError)) {
        throw;
      }
    }
  }
  throw std::runtime_error(lastError);
}
} // namespace

int main() {
  try {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    const std::string playerName = readPlayerName();
    Player player(playerName, 30, 5, 2);
    Game game(std::move(player));

    ActCatalog &rCatalog = game.getCatalog();
    rCatalog.add(ActAction("JOKE", "Vous racontez une blague absurde.", 15));
    rCatalog.add(ActAction("COMPLIMENT", "Vous flattez le monstre.", 12));
    rCatalog.add(ActAction("OBSERVE", "Vous observez attentivement.", 5));
    rCatalog.add(ActAction("DANCE", "Vous dansez de facon bizarre.", 10));
    rCatalog.add(ActAction("REASON", "Vous tentez de raisonner.", 8));
    rCatalog.add(ActAction("PET", "Vous donnez une petite caresse.", 14));
    rCatalog.add(ActAction("INSULT", "Vous lancez une pique acerbe.", -10));
    rCatalog.add(ActAction("THREATEN", "Vous menacez sans conviction.", -12));

    CsvLoader loader;
    Player &rPlayer = game.getPlayer();
    Inventory items = loadItemsWithFallback(loader);
    rPlayer.getInventory() = items;

    std::vector<std::unique_ptr<Monster>> monsters =
        loadMonstersWithFallback(loader, rCatalog);
    game.getMonsters() = std::move(monsters);

    game.showStartupSummary();
    game.run();
  } catch (const std::exception &rEx) {
    std::cerr << "Erreur: " << rEx.what() << "\n";
    return 1;
  }
  return 0;
}
