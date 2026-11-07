#include "DisplayUI.h"
#include "SerialProtocol.h"

DisplayUI display;
SerialProtocol protocol;

void handleCommand(const SerialCommand &command) {
  switch (command.type) {
    case SerialCommandType::Text:
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
  display.showWelcome();
  Serial.begin(115200);
}

void loop() {
  readSerialCommands();
}
