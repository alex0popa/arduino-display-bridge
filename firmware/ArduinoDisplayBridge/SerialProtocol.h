#ifndef SERIAL_PROTOCOL_H
#define SERIAL_PROTOCOL_H

#include <Arduino.h>

enum class SerialCommandType : uint8_t {
  Invalid,
  Text,
  StartGame,
  StopGame,
};

struct SerialCommand {
  SerialCommandType type;
  char line1[17];
  char line2[17];
};

class SerialProtocol {
 public:
  bool readNext(Stream &stream, SerialCommand &command);

 private:
  static const size_t kMaxMessageLength = 40;
  char message_[kMaxMessageLength + 1];
  size_t messageLength_ = 0;
  bool discardingMessage_ = false;

  void parseMessage(SerialCommand &command);
  static void copyRow(char *destination, const char *source, size_t length);
};

#endif
