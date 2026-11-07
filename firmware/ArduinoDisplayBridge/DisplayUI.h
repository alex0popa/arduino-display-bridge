#ifndef DISPLAY_UI_H
#define DISPLAY_UI_H

#include <Arduino.h>
#include <LiquidCrystal.h>

class DisplayUI {
 public:
  DisplayUI();

  void begin();
  void showWelcome();
  void showText(const char *line1, const char *line2);
  void drawGame(const uint8_t *snake, uint8_t length, uint8_t food);
  void showGameOver(uint8_t score);
  void showVictory();

 private:
  LiquidCrystal lcd_;
};

#endif
