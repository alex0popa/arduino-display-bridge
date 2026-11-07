#include "DisplayUI.h"

namespace {
constexpr uint8_t kLcdWidth = 16;
constexpr uint8_t kLcdHeight = 2;
}

DisplayUI::DisplayUI() : lcd_(12, 11, 5, 4, 3, 2) {}

void DisplayUI::begin() {
  lcd_.begin(kLcdWidth, kLcdHeight);
}

void DisplayUI::showWelcome() {
  showText("MCP ready", "Waiting for text");
}

void DisplayUI::showText(const char *line1, const char *line2) {
  lcd_.clear();
  lcd_.setCursor(0, 0);
  lcd_.print(line1);
  lcd_.setCursor(0, 1);
  lcd_.print(line2);
}
