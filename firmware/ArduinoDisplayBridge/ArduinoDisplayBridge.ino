// Main sketch: initializes the LCD, serial protocol, and Snake game, then dispatches commands and updates the game loop.
// Functions: drawCurrentGame renders game state; handleCommand dispatches parsed commands; readSerialCommands polls serial input; setup and loop are Arduino entry points.
#include "DisplayUI.h"
#include "SerialProtocol.h"
#include "SnakeGame.h"

DisplayUI display;
SerialProtocol protocol;
SnakeGame game;

void drawCurrentGame() {
  display.drawGame(game.body(), game.length(), game.food());
}

void handleCommand(const SerialCommand &command) {
  switch (command.type) {
    case SerialCommandType::StartGame:
      game.start(millis());
      drawCurrentGame();
      Serial.println("OK");
      break;
    case SerialCommandType::StopGame:
      game.stop();
      display.showWelcome();
      Serial.println("OK");
      break;
    case SerialCommandType::Text:
      game.stop();
      display.showText(command.line1, command.line2);
      Serial.println("OK");
      break;
    case SerialCommandType::Invalid:
      Serial.println("ERR");
      break;
  }
}

void readSerialCommands() {
  SerialCommand command;
  while (protocol.readNext(Serial, command)) {
    handleCommand(command);
  }
}

void setup() {
  display.begin();
  game.begin();
  randomSeed(analogRead(A2));
  display.showWelcome();
  Serial.begin(115200);
}

void loop() {
  readSerialCommands();

  if (game.buttonPressed() && !game.isRunning()) {
    game.start(millis());
    drawCurrentGame();
  }

  if (game.isRunning() && game.update(millis())) {
    if (game.isGameOver()) {
      display.showGameOver(game.score());
    } else if (game.isWon()) {
      display.showVictory();
    } else {
      drawCurrentGame();
    }
  }
}
