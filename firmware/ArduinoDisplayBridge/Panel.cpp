#include "Panel.h"

#include <string.h>

// Implements panel replacement, bounded viewport movement, and LCD row extraction.
void Panel::clear() {
  memset(cells_, ' ', sizeof(cells_));
  resetPosition();
}

void Panel::setRow(uint8_t row, const char *text) {
  memset(cells_[row], ' ', 48);
  const size_t length = strlen(text);
  memcpy(cells_[row], text, length);
}

void Panel::resetPosition() {
  column_ = 0;
  row_ = 0;
  centered_ = true;
}

bool Panel::update(uint32_t now) {
  (void)now;
  const int x = analogRead(A0);
  const int y = analogRead(A1);
  if (x >= 300 && x <= 700 && y >= 300 && y <= 700) {
    centered_ = true;
    return false;
  }
  if (!centered_) return false;
  centered_ = false;
  if (x < 300) {
    if (column_ > 0) --column_;
    else if (row_ > 0) { column_ = 2; --row_; }
    else return false;
  } else if (x > 700) {
    if (column_ < 2) ++column_;
    else if (row_ < 8) { column_ = 0; ++row_; }
    else return false;
  } else if (y < 300 && row_ > 0) {
    --row_;
  } else if (y > 700 && row_ < 8) {
    ++row_;
  } else {
    return false;
  }
  return true;
}

void Panel::copyVisibleRow(uint8_t screenRow, char *destination) const {
  memcpy(destination, cells_[row_ + screenRow] + column_ * 16, 16);
  destination[16] = '\0';
}
