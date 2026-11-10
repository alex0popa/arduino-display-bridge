# Arduino Firmware

This Arduino IDE sketch drives a 16×2 LCD and an optional joystick on an ELEGOO UNO R3. Arduino IDE compiles all sketch files in this directory together.

Purpose: Wire, upload, and understand the UNO firmware.
Status: Current.
Owner: Project maintainers.

## Hardware

### LCD wiring

| LCD pin | ELEGOO UNO R3 pin |
| --- | --- |
| RS | 12 |
| E | 11 |
| D4 | 5 |
| D5 | 4 |
| D6 | 3 |
| D7 | 2 |
| RW | GND |

### Joystick wiring

| Joystick pin | ELEGOO UNO R3 pin |
| --- | --- |
| GND | GND |
| +5V | 5V |
| VRX | A0 |
| VRY | A1 |
| SW | Digital pin 7 |

## Upload

Open `ArduinoDisplayBridge.ino` in Arduino IDE, select **Arduino Uno** and the board's USB port, then select **Verify** to compile or **Upload** to install the firmware. Close the Serial Monitor before using the [MCP server](../../docs/mcp-server/README.md).

Requirements: Arduino IDE and the `LiquidCrystal` library.

## Files

- `ArduinoDisplayBridge.ino`: setup, main loop, and coordination between the game, display, and serial protocol.
- `SnakeGame.h` and `SnakeGame.cpp`: Snake rules, joystick input, collisions, and score.
- `DisplayUI.h` and `DisplayUI.cpp`: LCD wiring and rendering.
- `Panel.h` and `Panel.cpp`: virtual 48×10 panel and joystick viewport navigation.
- `SerialProtocol.h` and `SerialProtocol.cpp`: parsing of text, panel, and game commands.

## Serial protocol

The UNO accepts newline-terminated ASCII commands over USB serial:

- `T|line1|line2` displays text.
- `B` clears the virtual panel for a new transfer.
- `P|row|text` writes row `0`–`9` (up to 48 characters).
- `F` shows the panel at its upper-left position.
- `G` starts Snake.
- `S` stops Snake.

Panel navigation uses one joystick movement per release back to center. In text or panel mode the center button does not start Snake; after game over or victory it restarts the game.

The firmware replies with `OK` when a command is handled and `ERR` when a command is invalid. Snake uses all 32 LCD cells; board edges and collisions end the game, and the initial move interval is approximately 450 ms.
