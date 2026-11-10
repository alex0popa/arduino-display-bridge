#ifndef VIRTUAL_TEXT_PANEL_H
#define VIRTUAL_TEXT_PANEL_H

// VirtualTextPanel owns a 48x10 character grid and its joystick-controlled 16x2 viewport.
// clear and setRow replace content; resetPosition resets navigation; update handles movement;
// copyVisibleRow extracts a display row.
#include <Arduino.h>

class VirtualTextPanel {
 public:
  void clear();
  void setRow(uint8_t row, const char *text);
  void resetPosition();
  bool update();
  void copyVisibleRow(uint8_t screenRow, char *destination) const;

 private:
  static const uint8_t kPanelRows = 10;
  static const uint8_t kPanelWidth = 48;
  static const uint8_t kDisplayRows = 2;
  static const uint8_t kDisplayWidth = 16;
  static const uint8_t kLastViewportRow = kPanelRows - kDisplayRows;
  static const uint8_t kLastViewportColumn = kPanelWidth / kDisplayWidth - 1;
  char cells_[kPanelRows][kPanelWidth];
  uint8_t column_ = 0;
  uint8_t row_ = 0;
  bool readyForNextMovement_ = true;
};

#endif
