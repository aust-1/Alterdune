#include "../../include/acts/ActAction.hpp"

ActAction::ActAction() : mId(""), mText(""), mMercyDelta(0) {}

ActAction::ActAction(const std::string &rId, const std::string &rText,
                     int mercyDelta)
    : mId(rId), mText(rText), mMercyDelta(mercyDelta) {}

const std::string &ActAction::getId() const { return mId; }

const std::string &ActAction::getText() const { return mText; }

int ActAction::getMercyDelta() const { return mMercyDelta; }
