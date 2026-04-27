#include "../../include/entities/Player.hpp"

Player::Player(const std::string &rName, int maxHp, int atk, int def)
    : Entity(rName, maxHp, atk, def), mKills(0), mSpares(0), mVictories(0) {}

int Player::getKills() const { return mKills; }

int Player::getSpares() const { return mSpares; }

int Player::getVictories() const { return mVictories; }

void Player::addKill() { ++mKills; }

void Player::addSpare() { ++mSpares; }

void Player::addVictory() { ++mVictories; }

void Player::heal(int amount) {
  if (amount <= 0) {
    return;
  }
  setHp(getHp() + amount);
}

Inventory &Player::getInventory() { return mInventory; }

const Inventory &Player::getInventory() const { return mInventory; }
