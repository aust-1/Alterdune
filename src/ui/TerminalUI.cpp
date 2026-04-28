#include "../../include/ui/TerminalUI.hpp"

#include <algorithm>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <iostream>

#ifdef _WIN32
#include <io.h>
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#else
#include <unistd.h>
#endif

namespace {
constexpr const char *kColorReset = "\033[0m";
constexpr const char *kColorDim = "\033[2m";
constexpr const char *kColorAccent = "\033[36m";
constexpr const char *kColorInfo = "\033[34m";
constexpr const char *kColorSuccess = "\033[32m";
constexpr const char *kColorWarning = "\033[33m";
constexpr const char *kColorError = "\033[31m";

bool isTruthyEnv(const char *rName) {
  const char *value = std::getenv(rName);
  if (value == nullptr) {
    return false;
  }
  return value[0] != '\0' && !(value[0] == '0' && value[1] == '\0');
}
} // namespace

TerminalUI::TerminalUI() : mUseColor(detectColorSupport()) {}

int TerminalUI::readMenuChoice() const {
  std::string line;
  if (!std::getline(std::cin, line)) {
    return kInputClosed;
  }
  const std::string cleaned = trim(line);
  if (cleaned.empty()) {
    return -1;
  }
  try {
    size_t consumed = 0;
    const int value = std::stoi(cleaned, &consumed);
    if (consumed != cleaned.size()) {
      return -1;
    }
    return value;
  } catch (const std::exception &) {
    return -1;
  }
}

void TerminalUI::printBanner(const std::string &rTitle,
                             const std::string &rSubtitle) const {
  const size_t innerWidth = 66;
  const std::string border = "+" + repeat('=', innerWidth) + "+";
  std::cout << "\n" << colorize(border, kColorAccent) << "\n";
  std::cout << colorize("|", kColorAccent) << alignCenter(rTitle, innerWidth)
            << colorize("|", kColorAccent) << "\n";
  if (!rSubtitle.empty()) {
    std::cout << colorize("|", kColorAccent)
              << alignCenter(rSubtitle, innerWidth)
              << colorize("|", kColorAccent) << "\n";
  }
  std::cout << colorize(border, kColorAccent) << "\n";
}

void TerminalUI::printSection(const std::string &rTitle) const {
  std::cout << colorize("\n-- " + rTitle + " " + repeat('-', 42), kColorDim)
            << "\n";
}

void TerminalUI::printMenu(const std::vector<std::string> &rOptions) const {
  for (size_t i = 0; i < rOptions.size(); ++i) {
    std::cout << "  " << (i + 1) << ") " << rOptions[i] << "\n";
  }
}

void TerminalUI::printPrompt(const std::string &rPrompt) const {
  std::cout << colorize(rPrompt, kColorAccent);
}

void TerminalUI::printMessage(MessageType type,
                              const std::string &rMessage) const {
  switch (type) {
  case MessageType::Info:
    std::cout << colorize("[INFO] ", kColorInfo) << rMessage << "\n";
    return;
  case MessageType::Success:
    std::cout << colorize("[OK]   ", kColorSuccess) << rMessage << "\n";
    return;
  case MessageType::Warning:
    std::cout << colorize("[WARN] ", kColorWarning) << rMessage << "\n";
    return;
  case MessageType::Error:
    std::cout << colorize("[ERR]  ", kColorError) << rMessage << "\n";
    return;
  }
}

void TerminalUI::printInfo(const std::string &rMessage) const {
  printMessage(MessageType::Info, rMessage);
}

void TerminalUI::printSuccess(const std::string &rMessage) const {
  printMessage(MessageType::Success, rMessage);
}

void TerminalUI::printWarning(const std::string &rMessage) const {
  printMessage(MessageType::Warning, rMessage);
}

void TerminalUI::printError(const std::string &rMessage) const {
  printMessage(MessageType::Error, rMessage);
}

void TerminalUI::printStat(const std::string &rLabel,
                           const std::string &rValue) const {
  std::cout << "  " << padRight(rLabel, 16) << " : " << rValue << "\n";
}

void TerminalUI::printProgressBar(const std::string &rLabel, int current,
                                  int maximum, int width) const {
  if (width < 8) {
    width = 8;
  }
  const int safeMax = std::max(1, maximum);
  const int clampedCurrent = std::clamp(current, 0, safeMax);
  const int filled = (clampedCurrent * width) / safeMax;
  const std::string bar = repeat('#', static_cast<size_t>(filled)) +
                          repeat('-', static_cast<size_t>(width - filled));
  std::cout << "  " << padRight(rLabel, 16) << " : [" << bar << "] "
            << clampedCurrent << "/" << safeMax << "\n";
}

void TerminalUI::printTable(
    const std::vector<std::string> &rHeaders,
    const std::vector<std::vector<std::string>> &rRows) const {
  if (rHeaders.empty()) {
    return;
  }

  std::vector<size_t> widths(rHeaders.size(), 0);
  for (size_t i = 0; i < rHeaders.size(); ++i) {
    widths[i] = rHeaders[i].size();
  }
  for (const std::vector<std::string> &rRow : rRows) {
    for (size_t i = 0; i < rHeaders.size() && i < rRow.size(); ++i) {
      widths[i] = std::max(widths[i], rRow[i].size());
    }
  }

  printTableBorder(widths, '=');
  std::cout << "|";
  for (size_t i = 0; i < rHeaders.size(); ++i) {
    std::cout << " " << padRight(rHeaders[i], widths[i]) << " |";
  }
  std::cout << "\n";
  printTableBorder(widths, '-');
  for (const std::vector<std::string> &rRow : rRows) {
    std::cout << "|";
    for (size_t i = 0; i < rHeaders.size(); ++i) {
      const std::string value = (i < rRow.size()) ? rRow[i] : "";
      std::cout << " " << padRight(value, widths[i]) << " |";
    }
    std::cout << "\n";
  }
  printTableBorder(widths, '=');
}

std::string TerminalUI::trim(const std::string &rText) {
  size_t start = 0;
  while (start < rText.size() &&
         std::isspace(static_cast<unsigned char>(rText[start]))) {
    ++start;
  }
  size_t end = rText.size();
  while (end > start &&
         std::isspace(static_cast<unsigned char>(rText[end - 1]))) {
    --end;
  }
  return rText.substr(start, end - start);
}

std::string TerminalUI::repeat(char value, size_t count) {
  return std::string(count, value);
}

std::string TerminalUI::padRight(const std::string &rText, size_t width) {
  if (rText.size() >= width) {
    return rText;
  }
  return rText + repeat(' ', width - rText.size());
}

std::string TerminalUI::alignCenter(const std::string &rText, size_t width) {
  if (rText.size() >= width) {
    return rText.substr(0, width);
  }
  const size_t padding = width - rText.size();
  const size_t left = padding / 2;
  const size_t right = padding - left;
  return repeat(' ', left) + rText + repeat(' ', right);
}

bool TerminalUI::detectColorSupport() const {
  if (isTruthyEnv("NO_COLOR")) {
    return false;
  }
  if (isTruthyEnv("CLICOLOR_FORCE")) {
    return true;
  }

#ifdef _WIN32
  if (_isatty(_fileno(stdout)) == 0) {
    return false;
  }
  HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
  if (handle != INVALID_HANDLE_VALUE) {
    DWORD mode = 0;
    if (GetConsoleMode(handle, &mode) != 0) {
      const DWORD withAnsi = mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING;
      if (SetConsoleMode(handle, withAnsi) != 0) {
        return true;
      }
    }
  }
  return std::getenv("WT_SESSION") != nullptr;
#else
  if (isatty(fileno(stdout)) == 0) {
    return false;
  }
  const char *term = std::getenv("TERM");
  if (term == nullptr) {
    return false;
  }
  const std::string termValue(term);
  return termValue != "dumb";
#endif
}

std::string TerminalUI::colorize(const std::string &rText,
                                 const char *rCode) const {
  if (!mUseColor) {
    return rText;
  }
  return std::string(rCode) + rText + kColorReset;
}

void TerminalUI::printTableBorder(const std::vector<size_t> &rWidths,
                                  char fill) const {
  std::string line;
  line += "+";
  for (size_t width : rWidths) {
    line += repeat(fill, width + 2);
    line += "+";
  }
  std::cout << colorize(line, kColorDim) << "\n";
}
