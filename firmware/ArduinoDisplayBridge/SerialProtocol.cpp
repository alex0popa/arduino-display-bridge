#include "SerialProtocol.h"

#include <string.h>

// Implements SerialProtocol: matchesCommand and startsWithPrefix recognize protocol tokens;
// isPanelRowMessage validates panel rows; readNext frames input; parseMessage maps commands;
// copyRow safely copies LCD rows.

namespace {
constexpr char kPanelRowPrefix[] = "P|";
constexpr char kTextPrefix[] = "T|";
constexpr char kBeginPanelCommand[] = "B";
constexpr char kShowPanelCommand[] = "F";
constexpr char kStartGameCommand[] = "G";
constexpr char kStopGameCommand[] = "S";
constexpr char kFieldSeparator = '|';
constexpr size_t kPanelRowNumberOffset = sizeof(kPanelRowPrefix) - 1;
constexpr size_t kPanelRowSeparatorOffset = kPanelRowNumberOffset + 1;
constexpr size_t kPanelRowTextOffset = kPanelRowSeparatorOffset + 1;

bool matchesCommand(const char *message, const char *command) {
  return strcmp(message, command) == 0;
}

bool startsWithPrefix(const char *message, const char *prefix) {
  return strncmp(message, prefix, strlen(prefix)) == 0;
}

bool isPanelRowMessage(const char *message) {
  if (!startsWithPrefix(message, kPanelRowPrefix)) {
    return false;
  }

  const char rowNumber = message[kPanelRowNumberOffset];
  const bool isValidRowNumber = rowNumber >= '0' && rowNumber <= '9';
  if (!isValidRowNumber) {
    return false;
  }
  if (message[kPanelRowSeparatorOffset] != kFieldSeparator) {
    return false;
  }

  const char *rowText = message + kPanelRowTextOffset;
  return strlen(rowText) <= SerialCommand::kPanelRowWidth;
}
}

bool SerialProtocol::readNext(Stream &stream, SerialCommand &command) {
  while (stream.available() > 0) {
    const char incoming = static_cast<char>(stream.read());
    const bool isCarriageReturn = incoming == '\r';
    if (isCarriageReturn) {
      continue;
    }
    const bool isMessageTerminator = incoming == '\n';
    if (isMessageTerminator) {
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
      const bool hasRoomForCharacter = messageLength_ < kMaxMessageLength;
      if (hasRoomForCharacter) {
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
  if (matchesCommand(message_, kBeginPanelCommand)) {
    command.type = SerialCommandType::PanelBegin;
    return;
  }
  if (matchesCommand(message_, kShowPanelCommand)) {
    command.type = SerialCommandType::PanelShow;
    return;
  }
  if (isPanelRowMessage(message_)) {
    command.row = message_[kPanelRowNumberOffset] - '0';
    strcpy(command.panelRow, message_ + kPanelRowTextOffset);
    command.type = SerialCommandType::PanelRow;
    return;
  }
  if (matchesCommand(message_, kStartGameCommand)) {
    command.type = SerialCommandType::StartGame;
    return;
  }
  if (matchesCommand(message_, kStopGameCommand)) {
    command.type = SerialCommandType::StopGame;
    return;
  }
  if (!startsWithPrefix(message_, kTextPrefix)) {
    command.type = SerialCommandType::Invalid;
    return;
  }

  const char *line1 = message_ + 2;
  const char *separator = strchr(line1, '|');
  const bool hasLineSeparator = separator != nullptr;
  if (!hasLineSeparator) {
    command.type = SerialCommandType::Invalid;
    return;
  }
  copyRow(command.line1, line1, static_cast<size_t>(separator - line1));
  copyRow(command.line2, separator + 1, strlen(separator + 1));
  command.type = SerialCommandType::Text;
}

void SerialProtocol::copyRow(char *destination, const char *source, size_t length) {
  if (length > SerialCommand::kLcdRowWidth) {
    length = SerialCommand::kLcdRowWidth;
  }
  memcpy(destination, source, length);
  destination[length] = '\0';
}
