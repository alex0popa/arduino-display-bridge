// Exercises panel serial framing and bounds using the production parser.
#include "SerialProtocol.h"
#include <cassert>
#include <string>

class Input : public Stream {
 public:
  explicit Input(const std::string &input) : input_(input) {}
  int available() override { return static_cast<int>(input_.size() - position_); }
  int read() override { return input_[position_++]; }
 private:
  std::string input_;
  size_t position_ = 0;
};

int main() {
  SerialProtocol protocol; SerialCommand command;
  Input input("B\nP|3|hello world\nF\nT|one|two\nG\nS\nP|0|" + std::string(49, 'x') + "\n");
  assert(protocol.readNext(input, command) && command.type == SerialCommandType::PanelBegin);
  assert(protocol.readNext(input, command) && command.type == SerialCommandType::PanelRow);
  assert(command.row == 3 && std::string(command.panelRow) == "hello world");
  assert(protocol.readNext(input, command) && command.type == SerialCommandType::PanelShow);
  assert(protocol.readNext(input, command) && command.type == SerialCommandType::Text);
  assert(std::string(command.line1) == "one" && std::string(command.line2) == "two");
  assert(protocol.readNext(input, command) && command.type == SerialCommandType::StartGame);
  assert(protocol.readNext(input, command) && command.type == SerialCommandType::StopGame);
  assert(protocol.readNext(input, command) && command.type == SerialCommandType::Invalid);
}
