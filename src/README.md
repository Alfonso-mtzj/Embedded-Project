
# src/

All firmware source code.

**What goes where**
- `main.c`: entry point + init + top-level loop
- `app/`: application state machine (mode select, dual-game switching)
- `game/`: pure Tic-Tac-Toe rules + AI (no hardware calls)
- `drivers/`: joystick/display/buzzer low-level code (hardware-facing)
- `ui/`: draws the board/cursor/status using the display driver
- `audio/`: sound effect patterns (move/error/win/tie)
- `utils/`: small reusable helpers (debounce, timing)
