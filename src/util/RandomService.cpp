#include "../../include/utils/RandomService.hpp"

#include <random>

int RandomService::rollDamage(int maxHp) const {
  if (maxHp <= 0) {
    return 0;
  }
  static thread_local std::mt19937 sRng(std::random_device{}());
  std::uniform_int_distribution<int> dist(0, maxHp);
  return dist(sRng);
}
