#ifndef SERIAL_PROTOCOL_H
#define SERIAL_PROTOCOL_H

// SerialCommandType and SerialCommand define the parsed command contract; SerialProtocol buffers, parses, and bounds serial messages.

#include <Arduino.h>

enum class SerialCommandType : uint8_t {
  Invalid,
  Text,
  PanelBegin,
  PanelRow,
  PanelShow,
  StartGame,
  StopGame,
};

struct SerialCommand {
  SerialCommandType type;
  uint8_t row;
  char panelRow[49];
  char line1[17];
  char line2[17];
};

class SerialProtocol {
 public:
  bool readNext(Stream &stream, SerialCommand &command);

 private:
  static const size_t kMaxMessageLength = 52;
  char message_[kMaxMessageLength + 1];
  size_t messageLength_ = 0;
  bool discardingMessage_ = false;

  void parseMessage(SerialCommand &command);
  static void copyRow(char *destination, const char *source, size_t length);
};

#endif
