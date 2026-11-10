# Navigate a virtual LCD panel

This ExecPlan is a living document. Progress, discoveries, edge cases, decisions, and outcomes are maintained below.

## Purpose / Big Picture

Send a 48-column by 10-row character panel from the MCP server to the UNO and explore its 16-by-2 window with the joystick. In text mode the center button must not start Snake; its existing restart action remains available after game over or victory.

## Progress

- [x] Identified and fixed the unintended button start; host simulation reproduced the issue and passed after the fix.
- [x] Implemented panel upload, rendering and joystick navigation.
- [x] Completed reviews, uploaded firmware and sent a test panel to the connected UNO.
- [x] User confirmed physical navigation works with a readable test message.

## Milestones

First implement a bounded panel and serial upload with existing simple text and game commands preserved. Next verify movement, boundaries and replacement, and document setup and tool usage.

## Surprises & Discoveries

- The Arduino CLI required an explicit library path. Compilation succeeded with 906 bytes of global RAM used.

## Edge Cases & Unspecified Behavior

- Panel rows may be shorter than 48 characters or omitted: fill their remaining cells with spaces. Covered by `tests/firmware/test_panel.cpp`.
- At vertical edges, movement stops. At horizontal edges wrapping changes the row by one only if a valid next window exists. Covered by `tests/firmware/test_panel.cpp`.
- A new panel resets position to the top left. Covered by `tests/firmware/test_panel.cpp` and `tests/firmware/test_sketch.cpp`.

## Decision Log

- Decision: Represent input as ten rows of up to 48 printable characters and preserve spaces. Rationale: this supports both independent columns and simple diagrams without automatic wrapping. Date: 2026-11-10.
- Decision: Keep existing text and game commands. Rationale: preserve established usage. Date: 2026-11-10.

## Outcomes & Retrospective

Panel and serial logic compile and host tests pass. The UNO accepted a test panel after upload, and the user confirmed navigation works with a readable test message.

## Context and Orientation

`server.py` sends newline-framed serial commands to `firmware/ArduinoDisplayBridge/SerialProtocol.cpp`. The sketch dispatches them and `DisplayUI` writes the 16-by-2 LCD. `SnakeGame` reads joystick axes A0 and A1 and button 7. The virtual panel is three 16-character columns by ten text rows; the window has a column index 0–2 and top row 0–8.

## Plan of Work

The panel, upload protocol, joystick handling, and module documentation are implemented. Rows sent through the MCP tool preserve spaces; the sender is responsible for making text readable across 16-character windows.

## Concrete Steps

From the repository root compile with `arduino-cli compile --libraries /Users/alex/Library/Arduino15/libraries --build-path /tmp/arduino-display-bridge-build --fqbn arduino:avr:uno firmware/ArduinoDisplayBridge`. Run the C++ host tests in `tests/firmware/` and `UV_CACHE_DIR=/tmp/arduino-uv-cache uv run --no-sync python -m unittest discover -s tests -p test_server.py -v`.

## Validation and Acceptance

A panel with distinct labels in each row and column must show the top-left 16-by-2 window. Up/down shifts one row. Left/right moves one column; crossing either side also moves one row in the corresponding direction, stopping if that is impossible. Uploading another panel resets the position. The button does nothing in text mode and can restart a completed game. Existing G, S and T commands continue to work.

## Idempotence and Recovery

Sending another panel replaces the current one. Rebuilds write to a temporary build directory. The connected UNO may be uploaded after compilation and review; no commit is performed without an explicit request.

## Artifacts and Notes

A temporary host simulation previously reproduced the unwanted button behavior and passed after the sketch fix.

## Interfaces and Dependencies

Python uses the existing MCP server and pyserial. Firmware uses the existing Arduino and LiquidCrystal libraries and must fit the UNO's memory.
