#pragma once

#include <map>
#include <string>
#include <vector>

#include "ActAction.hpp"

class ActCatalog {
public:
  void add(const ActAction &rAction);
  bool has(const std::string &rId) const;
  ActAction get(const std::string &rId) const;
  std::vector<ActAction> list() const;

private:
  std::map<std::string, ActAction> mActions;
};
