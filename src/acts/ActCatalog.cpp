#include "../../include/acts/ActCatalog.hpp"

void ActCatalog::add(const ActAction &rAction) {
  mActions[rAction.getId()] = rAction;
}

bool ActCatalog::has(const std::string &rId) const {
  return mActions.find(rId) != mActions.end();
}

ActAction ActCatalog::get(const std::string &rId) const {
  auto it = mActions.find(rId);
  if (it == mActions.end()) {
    return ActAction("", "", 0);
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
