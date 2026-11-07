"""Local MCP server for the UNO LCD and Snake game."""

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
    if len(candidates) != 1:
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


def _lcd_text(value: str) -> str:
    decomposed = unicodedata.normalize("NFKD", value)
    without_marks = "".join(char for char in decomposed if not unicodedata.combining(char))
    ascii_text = without_marks.encode("ascii", "replace").decode("ascii")
    printable = "".join(char if 32 <= ord(char) <= 126 and char != "|" else " " for char in ascii_text)
    return printable[:LCD_COLUMNS]


def _send_command(command: str) -> str:
    with _serial_lock:
        connection = _get_connection()
        try:
            connection.write(f"{command}\n".encode("ascii"))
            connection.flush()
            response = connection.readline().decode("ascii", errors="replace").strip()
        except serial.SerialException:
            connection.close()
            raise

        if response != "OK":
            raise RuntimeError(
                f"No valid confirmation from the UNO (response: {response or 'empty'}). "
                "Ensure the bridge firmware is uploaded and the Serial Monitor is closed."
            )
        return f"Command sent on port {connection.port}."


@server.tool()
def write_display(line1: str, line2: str) -> str:
    """Display two lines of text on the 16x2 LCD, stopping an active Snake game."""
    return _send_command(f"T|{_lcd_text(line1)}|{_lcd_text(line2)}")


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
