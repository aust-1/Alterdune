#include "../../include/bestiary/Bestiary.hpp"

void Bestiary::add(const BestiaryEntry &rEntry) { mEntries.push_back(rEntry); }

const std::vector<BestiaryEntry> &Bestiary::list() const { return mEntries; }
