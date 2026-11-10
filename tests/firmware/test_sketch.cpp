// Exercises command dispatch and button behavior in the production sketch.
#include <cassert>
#include <cstdint>
#include <cstring>
#define DISPLAY_UI_H
#define SERIAL_PROTOCOL_H
#define SNAKE_GAME_H
#define PANEL_H
constexpr int A2 = 2;
unsigned long millis() { return 0; }
int analogRead(int) { return 0; }
void randomSeed(int) {}
struct SerialClass { void println(const char*) {} void begin(int) {} } Serial;
enum class SerialCommandType { Invalid, Text, PanelBegin, PanelRow, PanelShow, StartGame, StopGame };
struct SerialCommand { SerialCommandType type; uint8_t row; char panelRow[49]; char line1[17]; char line2[17]; };
struct DisplayUI { void begin() {} void drawGame(const uint8_t*, uint8_t, uint8_t) {} void showWelcome() {} void showText(const char*, const char*) {} void showGameOver(uint8_t) {} void showVictory() {} };
struct SerialProtocol { bool readNext(SerialClass&, SerialCommand&) { return false; } };
struct Panel {
 int resets=0, clears=0, writes=0;
 void clear() { ++clears; } void setRow(uint8_t, const char*) { ++writes; }
 void resetPosition() { ++resets; } bool update(unsigned long) { return false; }
 void copyVisibleRow(uint8_t, char *out) { out[0]='\0'; }
};
struct SnakeGame {
 bool running=false, over=false, won=false, pressed=false; int starts=0;
 void begin() {} void start(unsigned long) { running=true; over=false; won=false; ++starts; } void stop() { running=false; over=false; won=false; }
 bool buttonPressed() { bool p=pressed; pressed=false; return p; } bool isRunning() const { return running; } bool isGameOver() const { return over; } bool isWon() const { return won; }
 bool update(unsigned long) { return false; } const uint8_t* body() { return nullptr; } uint8_t length() { return 0; } uint8_t food() { return 0; } uint8_t score() { return 0; }
};
#include "../../firmware/ArduinoDisplayBridge/ArduinoDisplayBridge.ino"
int main() {
 SerialCommand command{};
 command.type=SerialCommandType::Text; handleCommand(command);
 game.pressed=true; loop(); assert(game.starts==0 && !panelActive);
 command.type=SerialCommandType::PanelBegin; handleCommand(command); assert(panel.clears==1 && !panelActive);
 command.type=SerialCommandType::PanelRow; handleCommand(command); assert(panel.writes==1);
 command.type=SerialCommandType::PanelShow; handleCommand(command); assert(panelActive && panel.resets==1);
 game.pressed=true; loop(); assert(game.starts==0 && panelActive);
 command.type=SerialCommandType::StartGame; handleCommand(command); assert(game.starts==1 && !panelActive);
 game.running=false; game.over=true; game.pressed=true; loop(); assert(game.starts==2);
 game.running=false; game.over=false; game.won=true; game.pressed=true; loop(); assert(game.starts==3);
 command.type=SerialCommandType::Text; handleCommand(command); game.pressed=true; loop(); assert(game.starts==3);
}
