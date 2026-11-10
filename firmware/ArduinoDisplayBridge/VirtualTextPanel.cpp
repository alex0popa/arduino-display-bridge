#include "VirtualTextPanel.h"

#include <string.h>

// VirtualTextPanel implementation index:
// clear and setRow prepare panel content; resetPosition returns to the upper-left corner;
// update reads joystick movement and applies viewport boundaries;
// copyVisibleRow extracts the selected 16-character row for the LCD.
namespace {
constexpr uint8_t kJoystickXPin = A0;
constexpr uint8_t kJoystickYPin = A1;
constexpr int kJoystickLowThreshold = 300;
constexpr int kJoystickHighThreshold = 700;
}

void VirtualTextPanel::clear() {
  memset(cells_, ' ', sizeof(cells_));
  resetPosition();
}

void VirtualTextPanel::setRow(uint8_t row, const char *text) {
  memset(cells_[row], ' ', kPanelWidth);
  const size_t length = strlen(text);
  memcpy(cells_[row], text, length);
}

void VirtualTextPanel::resetPosition() {
  column_ = 0;
  row_ = 0;
  readyForNextMovement_ = true;
}

bool VirtualTextPanel::update() {
  const int x = analogRead(kJoystickXPin);
  const int y = analogRead(kJoystickYPin);
  const bool joystickIsCentered = x >= kJoystickLowThreshold && x <= kJoystickHighThreshold &&
                                  y >= kJoystickLowThreshold && y <= kJoystickHighThreshold;
  if (joystickIsCentered) {
    readyForNextMovement_ = true;
    return false;
  }
  if (!readyForNextMovement_) return false;
  readyForNextMovement_ = false;
  const bool moveLeft = x < kJoystickLowThreshold;
  const bool moveRight = x > kJoystickHighThreshold;
  const bool moveUp = y < kJoystickLowThreshold;
  const bool moveDown = y > kJoystickHighThreshold;
  const bool hasPreviousColumn = column_ > 0;
  const bool hasNextColumn = column_ < kLastViewportColumn;
  const bool hasPreviousRow = row_ > 0;
  const bool hasNextRow = row_ < kLastViewportRow;
  if (moveLeft) {
    if (hasPreviousColumn) --column_;
    else if (hasPreviousRow) { column_ = kLastViewportColumn; --row_; }
    else return false;
  } else if (moveRight) {
    if (hasNextColumn) ++column_;
    else if (hasNextRow) { column_ = 0; ++row_; }
    else return false;
  } else if (moveUp && hasPreviousRow) {
    --row_;
  } else if (moveDown && hasNextRow) {
    ++row_;
  } else {
    return false;
  }
  return true;
}

void VirtualTextPanel::copyVisibleRow(uint8_t screenRow, char *destination) const {
  memcpy(destination, cells_[row_ + screenRow] + column_ * kDisplayWidth, kDisplayWidth);
  destination[kDisplayWidth] = '\0';
}
