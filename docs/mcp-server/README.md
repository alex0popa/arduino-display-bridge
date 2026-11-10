# MCP Server

The root-level `server.py` exposes the Arduino LCD and Snake controls as MCP tools. It communicates with the ELEGOO UNO R3 over USB serial.

Purpose: Configure and use the Arduino Display MCP server.
Status: Current.
Owner: Project maintainers.

## Files

| File | Purpose |
| --- | --- |
| [`server.py`](../../server.py) | MCP entry point and tools for LCD and Snake commands over USB serial. |

## Requirements

- Python 3.11 or later
- [`uv`](https://docs.astral.sh/uv/)
- An ELEGOO UNO R3 (or compatible board) with the project firmware uploaded

## Setup

1. Install the Python environment from the repository root:

   ```sh
   uv sync
   ```

2. Close Arduino IDE's Serial Monitor. Only one application can use the serial port at a time.
3. Add the MCP server to `~/.codex/config.toml`, replacing paths with their locations on your Mac:

   ```toml
   [mcp_servers.arduino_display]
   command = "/path/to/uv"
   args = ["--directory", "/path/to/ArduinoDisplayBridge", "run", "server.py"]
   startup_timeout_sec = 30
   ```

The server automatically selects a serial device whose name contains `usbmodem`, `usbserial`, or `wchusbserial`. If multiple devices match or the board uses another name, set `ARDUINO_PORT`:

```toml
[mcp_servers.arduino_display.env]
ARDUINO_PORT = "/dev/cu.usbmodem1101"
```

The device name may change after reconnecting the board. The server opens the serial connection on the first command and keeps it open. Restart Codex after changing its MCP configuration or uploading updated firmware.

## Tools

### `write_display`

Displays two lines on the LCD and stops an active Snake game. Each line is limited to 16 printable ASCII characters. Characters with an ASCII decomposition, such as accented Latin letters, are converted to their base letter. Unsupported characters and the protocol delimiter are replaced with spaces.

Parameters: `line1`, `line2`.

### `start_snake`

Starts Snake. Press the joystick's center button (`SW`) to start or restart after game over; use the joystick directions to move.

### `stop_snake`

Stops Snake and returns the display to message mode.

See the [firmware README](../../firmware/ArduinoDisplayBridge/README.md) for hardware wiring and the serial protocol.
