# src/game/

Pure game logic (NO MSP430 registers, NO display calls, NO buzzer calls).

Put here:
- Board representation
- Apply move, validate move
- Win/tie detection
- Turn management
- AI move selection (PvC)

Keeping this hardware-free makes it easy to test and reuse.
