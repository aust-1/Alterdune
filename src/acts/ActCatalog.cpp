#include "../../include/acts/ActCatalog.hpp"

#include <stdexcept>

void ActCatalog::add(const ActAction &rAction) {
  mActions[rAction.getId()] = rAction;
}

bool ActCatalog::has(const std::string &rId) const {
  return mActions.find(rId) != mActions.end();
}

const ActAction &ActCatalog::get(const std::string &rId) const {
  auto it = mActions.find(rId);
  if (it == mActions.end()) {
    throw std::runtime_error("Unknown ACT id: " + rId);
  }
  return it->second;
}

std::vector<ActAction> ActCatalog::list() const {
  std::vector<ActAction> result;
  result.reserve(mActions.size());
  for (const auto &entry : mActions) {
    result.push_back(entry.second);
  }
  return result;
}
