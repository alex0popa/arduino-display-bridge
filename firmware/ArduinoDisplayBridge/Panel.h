#ifndef PANEL_H
#define PANEL_H

// Panel owns the virtual character grid and its joystick-controlled viewport.
#include <Arduino.h>

class Panel {
 public:
  void clear();
  void setRow(uint8_t row, const char *text);
  void resetPosition();
  bool update(uint32_t now);
  void copyVisibleRow(uint8_t screenRow, char *destination) const;

 private:
  char cells_[10][48];
  uint8_t column_ = 0;
  uint8_t row_ = 0;
  bool centered_ = true;
};

#endif
