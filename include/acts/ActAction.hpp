#pragma once

#include <string>

class ActAction {
public:
  ActAction();
  ActAction(const std::string &id, const std::string &text, int mercyDelta);

  const std::string &getId() const;
  const std::string &getText() const;
  int getMercyDelta() const;

private:
  std::string id;
  std::string text;
  int mercyDelta;
};
