#pragma once

#include <string>

class ActAction {
public:
  ActAction();
  ActAction(const std::string &rId, const std::string &rText, int mercyDelta);

  const std::string &getId() const;
  const std::string &getText() const;
  int getMercyDelta() const;

private:
  std::string mId;
  std::string mText;
  int mMercyDelta;
};
