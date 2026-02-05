#pragma once

#include <vector>

#include "BestiaryEntry.hpp"

class Bestiary {
public:
  void add(const BestiaryEntry &rEntry);
  const std::vector<BestiaryEntry> &list() const;

private:
  std::vector<BestiaryEntry> mEntries;
};
