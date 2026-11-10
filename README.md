# Arduino Display Bridge

A small local robotics project with three parts: an MCP server for the UNO, Arduino firmware for the LCD and joystick, and an optional local vision experiment for the ELEGOO camera.

Purpose: Orient readers to the project modules.
Status: Current.
Owner: Project maintainers.

## Modules

| Module | Purpose | Documentation |
| --- | --- | --- |
| MCP server (`server.py`) | Exposes LCD panel and Snake controls over MCP and USB serial. | [MCP server README](docs/mcp-server/README.md) |
| Arduino firmware (`firmware/ArduinoDisplayBridge/`) | Drives the 16×2 LCD, navigable panel, and joystick-controlled Snake game. | [Firmware README](firmware/ArduinoDisplayBridge/README.md) |
| Camera vision (`vision/`) | Samples the robot camera and asks a local Ollama vision model to describe each image. | [Vision README](vision/README.md) |

## Repository files

| File | Purpose |
| --- | --- |
| `AGENTS.md` | Repository workflow, structure, and content rules for coding agents. |
| `.gitignore` | Excludes local environments, generated files, and the private vision environment file. |
| `pyproject.toml` | Python project metadata, runtime requirements, and tool configuration. |
| `uv.lock` | Pins Python dependency versions; consult it when changing dependencies. |
| `server.py` | MCP server entry point; see the [MCP server README](docs/mcp-server/README.md). |
| `tests/` | Host-side firmware, server, and vision checks. |
| `exec_plans/` | Living plans for substantial changes. |

Python dependencies are managed with [`uv`](https://docs.astral.sh/uv/). Each module README contains its own setup and usage instructions.
