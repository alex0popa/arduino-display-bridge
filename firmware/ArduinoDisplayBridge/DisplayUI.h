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

 private:
  LiquidCrystal lcd_;
};

#endif
