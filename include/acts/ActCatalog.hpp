#pragma once

#include <map>
#include <string>
#include <vector>

#include "ActAction.hpp"

class ActCatalog {
public:
  void add(const ActAction &action);
  bool has(const std::string &id) const;
  ActAction get(const std::string &id) const;
  std::vector<ActAction> list() const;

private:
  std::map<std::string, ActAction> _actions;
};
