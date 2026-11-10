"""Local MCP server for the UNO LCD and Snake game.

Contents:
- `_resolve_port` and `_get_connection`: select and open the Arduino serial port.
- `_lcd_text`, `_send_command`, and `_send_commands`: normalize text and send framed commands.
- `write_display`, `write_panel`, `start_snake`, and `stop_snake`: expose MCP tools.
"""

from __future__ import annotations

import os
import time
import unicodedata
from threading import Lock

import serial
from mcp.server import MCPServer
from serial.tools import list_ports

BAUD_RATE = 115_200
LCD_COLUMNS = 16
PANEL_COLUMNS = 48
PANEL_ROWS = 10
_connection: serial.Serial | None = None
_serial_lock = Lock()
server = MCPServer("Arduino Display")


def _resolve_port() -> str:
    configured_port = os.environ.get("ARDUINO_PORT")
    if configured_port:
        return configured_port

    candidates = [
        port.device
        for port in list_ports.comports()
        if any(token in port.device.lower() for token in ("usbmodem", "usbserial", "wchusbserial"))
    ]
    has_unique_candidate = len(candidates) == 1
    if not has_unique_candidate:
        found = ", ".join(candidates) if candidates else "none"
        raise RuntimeError(
            f"Expected exactly one Arduino USB serial port; found: {found}. "
            "Set ARDUINO_PORT if the device uses a different name."
        )
    return candidates[0]


def _get_connection() -> serial.Serial:
    global _connection
    if _connection is not None and _connection.is_open:
        return _connection

    port = _resolve_port()
    _connection = serial.Serial(port, BAUD_RATE, timeout=3, write_timeout=3)
    time.sleep(2)
    _connection.reset_input_buffer()
    return _connection


def _lcd_text(value: str, limit: int = LCD_COLUMNS) -> str:
    decomposed = unicodedata.normalize("NFKD", value)
    without_marks = "".join(char for char in decomposed if not unicodedata.combining(char))
    ascii_text = without_marks.encode("ascii", "replace").decode("ascii")
    printable = "".join(char if 32 <= ord(char) <= 126 and char != "|" else " " for char in ascii_text)
    return printable[:limit]


def _send_commands(commands: list[str]) -> str:
    with _serial_lock:
        connection = _get_connection()
        try:
            for command in commands:
                connection.write(f"{command}\n".encode("ascii"))
                connection.flush()
                response = connection.readline().decode("ascii", errors="replace").strip()
                command_was_accepted = response == "OK"
                if not command_was_accepted:
                    connection.close()
                    raise RuntimeError(
                        f"No valid confirmation from the UNO (response: {response or 'empty'}). "
                        "Ensure the bridge firmware is uploaded and the Serial Monitor is closed."
                    )
        except serial.SerialException:
            connection.close()
            raise
        return f"Command sent on port {connection.port}."


def _send_command(command: str) -> str:
    return _send_commands([command])


@server.tool()
def write_display(line1: str, line2: str) -> str:
    """Display two lines of text on the 16x2 LCD, stopping an active Snake game."""
    return _send_command(f"T|{_lcd_text(line1)}|{_lcd_text(line2)}")


@server.tool()
def write_panel(rows: list[str]) -> str:
    """Show up to 10 rows of 48 characters in a joystick-navigable panel.

    Each group of 16 characters is a separate horizontal viewport. Spaces are
    significant, so pad content to align items in different columns.
    """
    has_too_many_rows = len(rows) > PANEL_ROWS
    has_invalid_row = not has_too_many_rows and any(
        len(row) > PANEL_COLUMNS or "\n" in row or "\r" in row for row in rows
    )
    if has_too_many_rows or has_invalid_row:
        raise ValueError("Panel accepts up to 10 single-line rows of at most 48 characters each.")
    commands = ["B"]
    commands.extend(f"P|{index}|{_lcd_text(row, PANEL_COLUMNS)}" for index, row in enumerate(rows))
    commands.append("F")
    return _send_commands(commands)


@server.tool()
def start_snake() -> str:
    """Start Snake; use the joystick to move and press SW to restart after game over."""
    return _send_command("G")


@server.tool()
def stop_snake() -> str:
    """Stop Snake and return the display to message mode."""
    return _send_command("S")


if __name__ == "__main__":
    server.run()
