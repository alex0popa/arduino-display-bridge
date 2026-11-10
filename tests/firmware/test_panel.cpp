// Exercises the actual panel navigation and viewport rendering on a host computer.
#include "Panel.h"
#include <cassert>
#include <cstring>
#include <string>

int x = 512, y = 512;
int analogRead(int pin) { return pin == A0 ? x : y; }

void move(Panel &panel, int nextX, int nextY, bool expected) {
  x = 512; y = 512; panel.update(0);
  x = nextX; y = nextY; assert(panel.update(0) == expected);
  assert(!panel.update(0));
}

void expect(Panel &panel, const char *first, const char *second) {
  char actual[17];
  panel.copyVisibleRow(0, actual); assert(std::string(actual) == first);
  panel.copyVisibleRow(1, actual); assert(std::string(actual) == second);
}

int main() {
  Panel panel; panel.clear();
  panel.setRow(0, "A000000000000000B000000000000000C000000000000000");
  panel.setRow(1, "A111111111111111B111111111111111C111111111111111");
  panel.setRow(2, "A222222222222222B222222222222222C222222222222222");
  expect(panel, "A000000000000000", "A111111111111111");
  move(panel, 0, 512, false);
  move(panel, 900, 512, true); move(panel, 900, 512, true);
  expect(panel, "C000000000000000", "C111111111111111");
  move(panel, 900, 512, true);
  expect(panel, "A111111111111111", "A222222222222222");
  move(panel, 0, 512, true);
  expect(panel, "C000000000000000", "C111111111111111");
  move(panel, 512, 900, true);
  expect(panel, "C111111111111111", "C222222222222222");
  for (int i = 0; i < 7; ++i) move(panel, 512, 900, true);
  move(panel, 512, 900, false); move(panel, 900, 512, false);
  panel.clear(); expect(panel, "                ", "                ");
}
