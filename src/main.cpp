#include "../include/io/CsvLoader.hpp"
#include "../include/systems/Game.hpp"

#include <iostream>

int main() {
  try {
    Player player("Hero", 30, 5, 2);
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
    Inventory items = loader.loadItems("data/items.csv");
    rPlayer.getInventory() = items;

    std::vector<std::unique_ptr<Monster>> monsters =
        loader.loadMonsters("data/monsters.csv", rCatalog);
    game.getMonsters() = std::move(monsters);

    game.run();
  } catch (const std::exception &rEx) {
    std::cerr << "Erreur: " << rEx.what() << "\n";
    return 1;
  }
  return 0;
}
