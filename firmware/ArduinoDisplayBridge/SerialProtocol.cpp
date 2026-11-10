#include "SerialProtocol.h"

#include <string.h>

// Implements SerialProtocol: readNext frames newline-delimited input; parseMessage maps commands; copyRow safely copies LCD rows.

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
  if (strcmp(message_, "B") == 0) {
    command.type = SerialCommandType::PanelBegin;
    return;
  }
  if (strcmp(message_, "F") == 0) {
    command.type = SerialCommandType::PanelShow;
    return;
  }
  if (message_[0] == 'P' && message_[1] == '|' &&
      message_[2] >= '0' && message_[2] <= '9' && message_[3] == '|' &&
      strlen(message_ + 4) <= 48) {
    command.row = message_[2] - '0';
    strcpy(command.panelRow, message_ + 4);
    command.type = SerialCommandType::PanelRow;
    return;
  }
  if (strcmp(message_, "G") == 0) {
    command.type = SerialCommandType::StartGame;
    return;
  }
  if (strcmp(message_, "S") == 0) {
    command.type = SerialCommandType::StopGame;
    return;
  }
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
