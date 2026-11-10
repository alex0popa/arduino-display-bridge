#ifndef ARDUINO_H
#define ARDUINO_H
#include <cstddef>
#include <cstdint>
constexpr int A0 = 0;
constexpr int A1 = 1;
int analogRead(int pin);
class Stream {
 public:
  virtual int available() = 0;
  virtual int read() = 0;
};
#endif
