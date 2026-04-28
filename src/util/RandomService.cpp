#include "../../include/utils/RandomService.hpp"

#include <cmath>
#include <random>

int RandomService::rollDamage(int atk, int def) const {
  const int safeAtk = atk < 1 ? 1 : atk;
  const int safeDef = def < 0 ? 0 : def;

  static thread_local std::mt19937 sRng(std::random_device{}());
  std::uniform_int_distribution<int> dist(0, safeAtk / 2);

  const int base = safeAtk + dist(sRng);
  const int reduction = static_cast<int>(std::floor(safeDef * 0.2));
  const int damage = base - reduction;
  if (damage < 1) {
    return 1;
  }
  return damage;
}
