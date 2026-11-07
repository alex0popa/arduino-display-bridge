# Arduino Display Bridge

A local MCP server that sends text to a 16×2 LCD connected to an ELEGOO UNO R3. The Mac communicates with the board over USB serial; no cloud services are used.

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

## Requirements

- Python 3.11 or later
- [`uv`](https://docs.astral.sh/uv/)
- Arduino IDE and the `LiquidCrystal` library
- An ELEGOO UNO R3 (or compatible board) and LCD

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

Displays two lines on the LCD. Each input line is limited to 16 printable ASCII characters. Characters with an ASCII decomposition, such as accented Latin letters, are converted to their base letter. Unsupported characters and the protocol delimiter are replaced with spaces.

Parameters: `line1`, `line2`.

## Serial protocol

The server sends the newline-terminated ASCII command `T|line1|line2` over USB serial to display text. The firmware replies with `OK` when a command is handled and `ERR` when a command is invalid. The server treats any response other than `OK` as a command failure.

## Firmware layout

The Arduino sketch files are in `firmware/ArduinoDisplayBridge/` and are compiled together by Arduino IDE:

- `ArduinoDisplayBridge.ino`: setup, main loop, and coordination between the display and serial protocol.
- `DisplayUI.h` and `DisplayUI.cpp`: LCD wiring and rendering.
- `SerialProtocol.h` and `SerialProtocol.cpp`: parsing of the `T` serial command.

After changing firmware, open the main `.ino` file in Arduino IDE, select **Verify** to compile, and **Upload** to install it. Keep the Serial Monitor closed while the MCP server is connected.
