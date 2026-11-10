// Main sketch: initializes the LCD, panel, serial protocol, and Snake game, then dispatches commands and updates input.
// Functions: drawPanel renders the viewport; drawCurrentGame renders game state; handleCommand dispatches parsed commands; readSerialCommands polls serial input; setup and loop are Arduino entry points.
#include "DisplayUI.h"
#include "VirtualTextPanel.h"
#include "SerialProtocol.h"
#include "SnakeGame.h"

DisplayUI display;
SerialProtocol protocol;
SnakeGame game;
VirtualTextPanel panel;
bool panelActive = false;

void drawPanel() {
  char line1[17];
  char line2[17];
  panel.copyVisibleRow(0, line1);
  panel.copyVisibleRow(1, line2);
  display.showText(line1, line2);
}

void drawCurrentGame() {
  display.drawGame(game.body(), game.length(), game.food());
}

void handleCommand(const SerialCommand &command) {
  switch (command.type) {
    case SerialCommandType::PanelBegin:
      game.stop();
      panelActive = false;
      panel.clear();
      Serial.println("OK");
      break;
    case SerialCommandType::PanelRow:
      panel.setRow(command.row, command.panelRow);
      Serial.println("OK");
      break;
    case SerialCommandType::PanelShow:
      game.stop();
      panel.resetPosition();
      panelActive = true;
      drawPanel();
      Serial.println("OK");
      break;
    case SerialCommandType::StartGame:
      panelActive = false;
      game.start(millis());
      drawCurrentGame();
      Serial.println("OK");
      break;
    case SerialCommandType::StopGame:
      panelActive = false;
      game.stop();
      display.showWelcome();
      Serial.println("OK");
      break;
    case SerialCommandType::Text:
      panelActive = false;
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

  if (game.buttonPressed() && (game.isGameOver() || game.isWon())) {
    game.start(millis());
    drawCurrentGame();
  }

  if (panelActive && panel.update()) {
    drawPanel();
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
