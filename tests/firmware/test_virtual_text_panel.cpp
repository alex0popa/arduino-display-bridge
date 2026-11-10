// Exercises the actual panel navigation and viewport rendering on a host computer.
#include "VirtualTextPanel.h"
#include <cassert>
#include <cstring>
#include <string>

constexpr int kJoystickCentered = 512;
constexpr int kJoystickLow = 0;
constexpr int kJoystickHigh = 900;
int x = kJoystickCentered, y = kJoystickCentered;
int analogRead(int pin) { return pin == A0 ? x : y; }

void moveJoystickAndExpect(VirtualTextPanel &panel, int nextX, int nextY, bool expected) {
  x = kJoystickCentered; y = kJoystickCentered; panel.update();
  x = nextX; y = nextY; assert(panel.update() == expected);
  assert(!panel.update());
}

void expect(VirtualTextPanel &panel, const char *first, const char *second) {
  char actual[17];
  panel.copyVisibleRow(0, actual); assert(std::string(actual) == first);
  panel.copyVisibleRow(1, actual); assert(std::string(actual) == second);
}

int main() {
  VirtualTextPanel panel; panel.clear();
  panel.setRow(0, "A000000000000000B000000000000000C000000000000000");
  panel.setRow(1, "A111111111111111B111111111111111C111111111111111");
  panel.setRow(2, "A222222222222222B222222222222222C222222222222222");
  expect(panel, "A000000000000000", "A111111111111111");
  // The first position cannot move left; two right moves reach the last column.
  moveJoystickAndExpect(panel, kJoystickLow, kJoystickCentered, false);
  moveJoystickAndExpect(panel, kJoystickHigh, kJoystickCentered, true);
  moveJoystickAndExpect(panel, kJoystickHigh, kJoystickCentered, true);
  expect(panel, "C000000000000000", "C111111111111111");
  // Crossing the right edge advances one row and returns to the first column.
  moveJoystickAndExpect(panel, kJoystickHigh, kJoystickCentered, true);
  expect(panel, "A111111111111111", "A222222222222222");
  // Crossing the left edge reverses that movement.
  moveJoystickAndExpect(panel, kJoystickLow, kJoystickCentered, true);
  expect(panel, "C000000000000000", "C111111111111111");
  // Vertical movement advances one row, retaining the current column.
  moveJoystickAndExpect(panel, kJoystickCentered, kJoystickHigh, true);
  expect(panel, "C111111111111111", "C222222222222222");
  // The final row blocks downward movement and wrapping past the right edge.
  for (int i = 0; i < 7; ++i) moveJoystickAndExpect(panel, kJoystickCentered, kJoystickHigh, true);
  moveJoystickAndExpect(panel, kJoystickCentered, kJoystickHigh, false);
  moveJoystickAndExpect(panel, kJoystickHigh, kJoystickCentered, false);
  panel.clear(); expect(panel, "                ", "                ");
}
