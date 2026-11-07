#include "SerialProtocol.h"

#include <string.h>

bool SerialProtocol::readNext(Stream &stream, SerialCommand &command) {
  while (stream.available() > 0) {
    const char incoming = static_cast<char>(stream.read());
    if (incoming == '\r') {
      continue;
    }
    if (incoming == '\n') {
      if (discardingMessage_) {
        command.type = SerialCommandType::Invalid;
      } else {
        message_[messageLength_] = '\0';
        parseMessage(command);
      }
      messageLength_ = 0;
      discardingMessage_ = false;
      return true;
    }
    if (!discardingMessage_) {
      if (messageLength_ < kMaxMessageLength) {
        message_[messageLength_++] = incoming;
      } else {
        discardingMessage_ = true;
      }
    }
  }
  return false;
}

void SerialProtocol::parseMessage(SerialCommand &command) {
  command.line1[0] = '\0';
  command.line2[0] = '\0';
  if (message_[0] != 'T' || message_[1] != '|') {
    command.type = SerialCommandType::Invalid;
    return;
  }

  const char *line1 = message_ + 2;
  const char *separator = strchr(line1, '|');
  if (separator == nullptr) {
    command.type = SerialCommandType::Invalid;
    return;
  }
  copyRow(command.line1, line1, static_cast<size_t>(separator - line1));
  copyRow(command.line2, separator + 1, strlen(separator + 1));
  command.type = SerialCommandType::Text;
}

void SerialProtocol::copyRow(char *destination, const char *source, size_t length) {
  if (length > 16) {
    length = 16;
  }
  memcpy(destination, source, length);
  destination[length] = '\0';
}
