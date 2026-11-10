# Repository Instructions

Repository-wide rules for contributors and coding agents, including language, project structure, per-file indexing, implementation workflow, configuration, firmware, and Git conventions.

## Language

- Write all maintained repository content in English, including identifiers, comments, user-facing text, documentation, and commit messages.
- Preserve external protocol tokens, hardware pin names, and third-party API identifiers when they are compatibility contracts.

## Project Structure

- Keep Arduino sketch files together in `firmware/ArduinoDisplayBridge/`; Arduino IDE compiles the sketch files in that directory together.
- Keep the MCP server entry point in `server.py` unless a structural change is required.
- Give each module its own README for setup and usage; keep the root README as a concise project index linking to module READMEs.
- Maintain a concise index for every maintained file where the format supports it, especially source files with multiple functions, classes, or major sections. Source code files carry a module docstring or leading comment naming each major entity and its role; very small files need only a one-line purpose statement. For structured formats that support comments, use a short in-file purpose note when useful; when comments are unsupported or would add noise, index the file in its module README. Keep hierarchical navigation: the root README maps modules and shared project files, and each module README lists its files and points to important entry points or contracts. Update an index when a change alters the documented purpose, major entities or sections, role, or contract—not for implementation edits that leave that map accurate. Describe roles and relationships, not full implementations. Exclude generated, vendored, lock, binary, cache, and local environment files unless their role is important; group those by category rather than describing every item.
- Keep generated environments, caches, build outputs, and local configuration out of version control.

## Python

- Manage the environment and dependencies with `uv`.
- Use type annotations and clear, domain-specific names.
- Keep serial protocol handling aligned with the Arduino firmware.
- Keep environment-specific configuration, including service URLs, model identifiers, credentials, and deployment settings, out of source code. Read it from environment variables and document the variable names and required status. Protocol constants and product behavior may remain in code.

## Firmware

- Preserve the existing LCD and joystick pin assignments unless the user requests a hardware change.
- Keep serial command framing compatible between `server.py` and `SerialProtocol.cpp`.

## Git

- Use focused commits with English subjects and conventional prefixes such as `Feat`, `Fix`, `Chore`, `Refactor`, `Test`, or `Docs`.
- Do not create commits unless the user explicitly asks for them.
