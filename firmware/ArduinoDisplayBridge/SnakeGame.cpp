#include "SnakeGame.h"

namespace {
constexpr uint8_t kBoardWidth = 16;
constexpr uint8_t kJoystickXPin = A0;
constexpr uint8_t kJoystickYPin = A1;
constexpr uint8_t kJoystickButtonPin = 7;
constexpr uint16_t kMoveIntervalMs = 450;
constexpr uint16_t kInputIntervalMs = 70;
constexpr int kJoystickLowThreshold = 300;
constexpr int kJoystickHighThreshold = 700;
}

void SnakeGame::begin() {
  pinMode(kJoystickButtonPin, INPUT_PULLUP);
}

void SnakeGame::start(uint32_t now) {
  running_ = true;
  gameOver_ = false;
  score_ = 0;
  length_ = 3;
  snake_[0] = 4;
  snake_[1] = 3;
  snake_[2] = 2;
  direction_ = Direction::Right;
  lastMoveAt_ = now;
  lastInputAt_ = now;
  placeFood();
}

void SnakeGame::stop() {
  running_ = false;
  gameOver_ = false;
}

bool SnakeGame::buttonPressed() {
  const bool buttonState = digitalRead(kJoystickButtonPin);
  const bool pressed = previousButtonState_ == HIGH && buttonState == LOW;
  previousButtonState_ = buttonState;
  return pressed;
}

bool SnakeGame::update(uint32_t now) {
  if (!running_) {
    return false;
  }

  readJoystick(now);
  if (now - lastMoveAt_ < kMoveIntervalMs) {
    return false;
  }

  lastMoveAt_ = now;
  advance();
  return true;
}

bool SnakeGame::isRunning() const {
  return running_;
}

bool SnakeGame::isGameOver() const {
  return gameOver_;
}

uint8_t SnakeGame::length() const {
  return length_;
}

const uint8_t *SnakeGame::body() const {
  return snake_;
}

uint8_t SnakeGame::food() const {
  return food_;
}

uint8_t SnakeGame::score() const {
  return score_;
}

void SnakeGame::readJoystick(uint32_t now) {
  if (now - lastInputAt_ < kInputIntervalMs) {
    return;
  }
  lastInputAt_ = now;

  const int x = analogRead(kJoystickXPin);
  const int y = analogRead(kJoystickYPin);
  Direction requestedDirection = direction_;
  if (x < kJoystickLowThreshold) {
    requestedDirection = Direction::Left;
  } else if (x > kJoystickHighThreshold) {
    requestedDirection = Direction::Right;
  } else if (y < kJoystickLowThreshold) {
    requestedDirection = Direction::Up;
  } else if (y > kJoystickHighThreshold) {
    requestedDirection = Direction::Down;
  }

  const uint8_t requested = static_cast<uint8_t>(requestedDirection);
  const uint8_t current = static_cast<uint8_t>(direction_);
  if (requested != current && (requested + 2) % 4 != current) {
    direction_ = requestedDirection;
  }
}

void SnakeGame::advance() {
  const uint8_t head = snake_[0];
  uint8_t next = head;
  switch (direction_) {
    case Direction::Up:
      if (head < kBoardWidth) {
        running_ = false;
        gameOver_ = true;
        return;
      }
      next -= kBoardWidth;
      break;
    case Direction::Right:
      if (head % kBoardWidth == kBoardWidth - 1) {
        running_ = false;
        gameOver_ = true;
        return;
      }
      ++next;
      break;
    case Direction::Down:
      if (head >= kCellCount - kBoardWidth) {
        running_ = false;
        gameOver_ = true;
        return;
      }
      next += kBoardWidth;
      break;
    case Direction::Left:
      if (head % kBoardWidth == 0) {
        running_ = false;
        gameOver_ = true;
        return;
      }
      --next;
      break;
  }

  const bool ateFood = next == food_;
  const uint8_t cellsToCheck = ateFood ? length_ : length_ - 1;
  if (contains(next, cellsToCheck)) {
    running_ = false;
    gameOver_ = true;
    return;
  }

  if (ateFood) {
    ++length_;
    ++score_;
  }
  for (uint8_t i = length_ - 1; i > 0; --i) {
    snake_[i] = snake_[i - 1];
  }
  snake_[0] = next;

  if (ateFood) {
    placeFood();
  }
}

bool SnakeGame::contains(uint8_t cell, uint8_t cellsToCheck) const {
  for (uint8_t i = 0; i < cellsToCheck; ++i) {
    if (snake_[i] == cell) {
      return true;
    }
  }
  return false;
}

void SnakeGame::placeFood() {
  if (length_ >= kCellCount) {
    running_ = false;
    gameOver_ = true;
    return;
  }

  do {
    food_ = static_cast<uint8_t>(random(kCellCount));
  } while (contains(food_, length_));
}
