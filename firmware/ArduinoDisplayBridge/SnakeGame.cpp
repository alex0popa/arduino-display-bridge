#include "SnakeGame.h"

// Implements SnakeGame: lifecycle/state accessors, joystick sampling, timed movement, collision checks, and food placement.
// Functions: begin/start/stop configure lifecycle; buttonPressed and readJoystick handle input; update/advance run gameplay; accessors expose state; isOccupiedBySnake and placeFood implement board logic.

namespace {
constexpr uint8_t kBoardWidth = 16;
constexpr uint8_t kJoystickXPin = A0;
constexpr uint8_t kJoystickYPin = A1;
constexpr uint8_t kJoystickButtonPin = 7;
constexpr uint16_t kMoveIntervalMs = 450;
constexpr uint16_t kInputIntervalMs = 70;
constexpr int kJoystickLowThreshold = 300;
constexpr int kJoystickHighThreshold = 700;
constexpr uint8_t kDirectionCount = 4;
constexpr uint8_t kOppositeDirectionOffset = 2;
}

void SnakeGame::begin() {
  pinMode(kJoystickButtonPin, INPUT_PULLUP);
}

void SnakeGame::start(uint32_t now) {
  running_ = true;
  gameOver_ = false;
  won_ = false;
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
  won_ = false;
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
  const bool movementIsDue = now - lastMoveAt_ >= kMoveIntervalMs;
  if (!movementIsDue) {
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

bool SnakeGame::isWon() const {
  return won_;
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
  const bool inputIsDue = now - lastInputAt_ >= kInputIntervalMs;
  if (!inputIsDue) {
    return;
  }
  lastInputAt_ = now;

  const int x = analogRead(kJoystickXPin);
  const int y = analogRead(kJoystickYPin);
  const bool moveLeft = x < kJoystickLowThreshold;
  const bool moveRight = x > kJoystickHighThreshold;
  const bool moveUp = y < kJoystickLowThreshold;
  const bool moveDown = y > kJoystickHighThreshold;
  Direction requestedDirection = direction_;
  if (moveLeft) {
    requestedDirection = Direction::Left;
  } else if (moveRight) {
    requestedDirection = Direction::Right;
  } else if (moveUp) {
    requestedDirection = Direction::Up;
  } else if (moveDown) {
    requestedDirection = Direction::Down;
  }

  const uint8_t requested = static_cast<uint8_t>(requestedDirection);
  const uint8_t current = static_cast<uint8_t>(direction_);
  // Directions are ordered clockwise, so opposite directions are two steps apart.
  const bool isOppositeDirection = (requested + kOppositeDirectionOffset) % kDirectionCount == current;
  if (requested != current && !isOppositeDirection) {
    direction_ = requestedDirection;
  }
}

void SnakeGame::advance() {
  const uint8_t head = snake_[0];
  const bool atTopEdge = head < kBoardWidth;
  const bool atRightEdge = head % kBoardWidth == kBoardWidth - 1;
  const bool atBottomEdge = head >= kCellCount - kBoardWidth;
  const bool atLeftEdge = head % kBoardWidth == 0;
  uint8_t next = head;
  switch (direction_) {
    case Direction::Up:
      if (atTopEdge) {
        running_ = false;
        gameOver_ = true;
        return;
      }
      next -= kBoardWidth;
      break;
    case Direction::Right:
      if (atRightEdge) {
        running_ = false;
        gameOver_ = true;
        return;
      }
      ++next;
      break;
    case Direction::Down:
      if (atBottomEdge) {
        running_ = false;
        gameOver_ = true;
        return;
      }
      next += kBoardWidth;
      break;
    case Direction::Left:
      if (atLeftEdge) {
        running_ = false;
        gameOver_ = true;
        return;
      }
      --next;
      break;
  }

  const bool ateFood = next == food_;
  // The tail vacates its cell unless the snake grows after eating.
  const uint8_t bodySegmentsToCheck = ateFood ? length_ : length_ - 1;
  if (isOccupiedBySnake(next, bodySegmentsToCheck)) {
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

bool SnakeGame::isOccupiedBySnake(uint8_t cell, uint8_t bodySegmentsToCheck) const {
  for (uint8_t i = 0; i < bodySegmentsToCheck; ++i) {
    if (snake_[i] == cell) {
      return true;
    }
  }
  return false;
}

void SnakeGame::placeFood() {
  if (length_ >= kCellCount) {
    running_ = false;
    won_ = true;
    return;
  }

  do {
    food_ = static_cast<uint8_t>(random(kCellCount));
  } while (isOccupiedBySnake(food_, length_));
}
