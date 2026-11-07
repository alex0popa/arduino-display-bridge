# Arduino Display Bridge

A local MCP server that sends text and Snake game commands to a 16×2 LCD connected to an ELEGOO UNO R3. The Mac communicates with the board over USB serial; no cloud services are used.

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
| SW | Arduino digital pin 7 |

## Requirements

- Python 3.11 or later
- [`uv`](https://docs.astral.sh/uv/)
- Arduino IDE and the `LiquidCrystal` library
- An ELEGOO UNO R3 (or compatible board), LCD, and optional joystick

## Setup

1. Open `firmware/ArduinoDisplayBridge/ArduinoDisplayBridge.ino` in Arduino IDE, select **Arduino Uno** and the board's USB port, then upload the sketch.
2. Close Arduino IDE's Serial Monitor. Only one application can use the serial port at a time.
3. Install the Python environment from the project root:

   ```sh
   uv sync
   ```

4. Add the MCP server to `~/.codex/config.toml`, replacing the paths with the locations on your machine:

   ```toml
   [mcp_servers.arduino_display]
   command = "/path/to/uv"
   args = ["--directory", "/path/to/ArduinoDisplayBridge", "run", "server.py"]
   startup_timeout_sec = 30
   ```

The server automatically selects a serial device whose name contains `usbmodem`, `usbserial`, or `wchusbserial`. If multiple devices match or the board uses another name, configure the port:

```toml
[mcp_servers.arduino_display.env]
ARDUINO_PORT = "/dev/cu.usbmodem1101"
```

The device name may change after reconnecting the board. The server opens the serial connection on the first command and keeps it open. Restart Codex after changing its MCP configuration or uploading updated firmware.

## MCP tools

### `write_display`

Displays two lines on the LCD and stops an active Snake game. Each input line is limited to 16 printable ASCII characters. Characters with an ASCII decomposition, such as accented Latin letters, are converted to their base letter. Unsupported characters and the protocol delimiter are replaced with spaces.

Parameters: `line1`, `line2`.

### `start_snake`

Starts a Snake game. Press the joystick's center button (`SW`) to start or restart after game over; use the joystick directions to move.

### `stop_snake`

Stops Snake and returns the display to message mode.

## Serial protocol

The server sends newline-terminated ASCII commands over USB serial: `T|line1|line2` displays text, `G` starts Snake, and `S` stops it. The firmware replies with `OK` when a command is handled and `ERR` when a command is invalid. The server treats any response other than `OK` as a command failure.

Snake uses all 32 LCD cells. Board edges and collisions end the game; the initial move interval is approximately 450 ms.

## Firmware layout

All Arduino sketch files are in `firmware/ArduinoDisplayBridge/` and are compiled together by Arduino IDE:

- `ArduinoDisplayBridge.ino`: setup, main loop, and coordination between the game, display, and serial protocol.
- `SnakeGame.h` and `SnakeGame.cpp`: Snake rules, joystick input, collisions, and score.
- `DisplayUI.h` and `DisplayUI.cpp`: LCD wiring and rendering.
- `SerialProtocol.h` and `SerialProtocol.cpp`: parsing of the `T`, `G`, and `S` serial commands.

After changing firmware, open the main `.ino` file in Arduino IDE, select **Verify** to compile, and **Upload** to install it. Keep the Serial Monitor closed while the MCP server is connected.
