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

void DisplayUI::drawGame(const uint8_t *snake, uint8_t length, uint8_t food) {
  lcd_.clear();
  for (uint8_t i = 0; i < length; ++i) {
    const uint8_t cell = snake[i];
    lcd_.setCursor(cell % kLcdWidth, cell / kLcdWidth);
    lcd_.write(i == 0 ? 'O' : 'o');
  }
  lcd_.setCursor(food % kLcdWidth, food / kLcdWidth);
  lcd_.write('*');
}

void DisplayUI::showGameOver(uint8_t score) {
  lcd_.clear();
  lcd_.setCursor(0, 0);
  lcd_.print("Game over: ");
  lcd_.print(score);
  lcd_.setCursor(0, 1);
  lcd_.print("Press SW to play");
}

void DisplayUI::showVictory() {
  showText("YOU WIN!", "Board full");
}
