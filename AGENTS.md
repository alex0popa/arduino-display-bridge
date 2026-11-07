# Repository Instructions

## Language

- Write all maintained repository content in English, including identifiers, comments, user-facing text, documentation, and commit messages.
- Preserve external protocol tokens, hardware pin names, and third-party API identifiers when they are compatibility contracts.

## Project Structure

- Keep Arduino sketch files together in `firmware/ArduinoDisplayBridge/`; Arduino IDE compiles the sketch files in that directory together.
- Keep the MCP server entry point in `server.py` unless a structural change is required.
- Keep generated environments, caches, build outputs, and local configuration out of version control.

## Python

- Manage the environment and dependencies with `uv`.
- Use type annotations and clear, domain-specific names.
- Keep serial protocol handling aligned with the Arduino firmware.

## Firmware

- Preserve the existing LCD and joystick pin assignments unless the user requests a hardware change.
- Keep serial command framing compatible between `server.py` and `SerialProtocol.cpp`.

## Git

- Use focused commits with English subjects and conventional prefixes such as `Feat`, `Fix`, `Chore`, `Refactor`, `Test`, or `Docs`.
- Do not create commits unless the user explicitly asks for them.
