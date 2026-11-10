#ifndef SNAKE_GAME_H
#define SNAKE_GAME_H

// SnakeGame declares the gameplay API, state accessors, board representation, and private input/movement state.

#include <Arduino.h>

class SnakeGame {
 public:
  void begin();
  void start(uint32_t now);
  void stop();
  bool buttonPressed();
  bool update(uint32_t now);

  bool isRunning() const;
  bool isGameOver() const;
  bool isWon() const;
  uint8_t length() const;
  const uint8_t *body() const;
  uint8_t food() const;
  uint8_t score() const;

 private:
  enum class Direction : uint8_t {
    Up,
    Right,
    Down,
    Left,
  };

  static const uint8_t kCellCount = 32;
  uint8_t snake_[kCellCount];
  uint8_t length_ = 3;
  uint8_t food_ = 0;
  uint8_t score_ = 0;
  Direction direction_ = Direction::Right;
  uint32_t lastMoveAt_ = 0;
  uint32_t lastInputAt_ = 0;
  bool running_ = false;
  bool gameOver_ = false;
  bool won_ = false;
  bool previousButtonState_ = HIGH;

  void readJoystick(uint32_t now);
  void advance();
  bool isOccupiedBySnake(uint8_t cell, uint8_t bodySegmentsToCheck) const;
  void placeFood();
};

#endif
