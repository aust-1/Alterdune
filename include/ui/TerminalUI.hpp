#pragma once

#include <cstddef>
#include <limits>
#include <string>
#include <vector>

class TerminalUI {
public:
  static constexpr int kInputClosed = std::numeric_limits<int>::min();

  enum class MessageType { Info, Success, Warning, Error };

  TerminalUI();

  int readMenuChoice() const;

  void printBanner(const std::string &rTitle,
                   const std::string &rSubtitle = "") const;
  void printSection(const std::string &rTitle) const;
  void printMenu(const std::vector<std::string> &rOptions) const;
  void printPrompt(const std::string &rPrompt) const;

  void printMessage(MessageType type, const std::string &rMessage) const;
  void printInfo(const std::string &rMessage) const;
  void printSuccess(const std::string &rMessage) const;
  void printWarning(const std::string &rMessage) const;
  void printError(const std::string &rMessage) const;

  void printStat(const std::string &rLabel, const std::string &rValue) const;
  void printProgressBar(const std::string &rLabel, int current, int maximum,
                        int width = 24) const;
  void printTable(const std::vector<std::string> &rHeaders,
                  const std::vector<std::vector<std::string>> &rRows) const;

private:
  bool mUseColor;

  static std::string trim(const std::string &rText);
  static std::string repeat(char value, size_t count);
  static std::string padRight(const std::string &rText, size_t width);
  static std::string alignCenter(const std::string &rText, size_t width);

  bool detectColorSupport() const;
  std::string colorize(const std::string &rText, const char *rCode) const;
  void printTableBorder(const std::vector<size_t> &rWidths, char fill) const;
};
