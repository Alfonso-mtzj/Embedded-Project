# Embedded Project — Dual Tic‑Tac‑Toe (MSP430FR6989 + BOOSTXL‑EDUMKII)

This repo contains a simple embedded C application that runs a Tic‑Tac‑Toe game on the **Educational BoosterPack MKII (BOOSTXL‑EDUMKII)** connected to the **MSP‑EXP430FR6989 LaunchPad**.

The design is intentionally modular (drivers → game logic → UI → app state machine) so it’s easy to build, debug, and explain.

---

## Features
- Tic‑Tac‑Toe game logic (win/tie detection)
- PvP and PvC mode (computer AI chooses moves)
- Joystick controls for cursor movement + select
- Buzzer sound effects (move / win / tie / error)
- 128×128 TFT display rendering (grid, pieces, cursor)

---

## Folder Structure (High Level)
- `src/main.c`  
  Entry point. Initializes hardware and runs the main loop.

- `src/app/`  
  **Application state machine** (`game_manager.c`) — handles menu vs playing, reads joystick actions, triggers sound, calls rendering.

- `src/drivers/`  
  **Hardware drivers** (no game logic):
  - `display.c/.h` — TFT driver exposing `Display_*` drawing primitives  
  - `joystick.c/.h` — ADC joystick + SEL button → returns `JoyAction`
  - `buzzer.c/.h` — buzzer output (tone/beep support)

- `src/game/`  
  **Pure game logic** (no hardware):
  - `tictactoe.c/.h` — board state, apply move, check win/tie
  - `ai.c/.h` — computer move selection

- `src/ui/`  
  **Rendering only**:
  - `render.c/.h` — draws grid/pieces/cursor using `Display_*`

- `src/audio/`  
  **Sound effects patterns**:
  - `sfx.c/.h` — “move”, “win”, “tie”, “error”, “switch” sounds

---

## Control Mapping (BoosterPack)
- **Joystick X/Y**: analog inputs (ADC)  
- **Joystick SEL (press)**: digital input (active‑low pull‑up)
- **Buzzer**: BoosterPack buzzer output pin
- **TFT**: driven via SPI

> Note: Exact MCU pin mapping depends on the lab pin map and is configured inside the driver files in `src/drivers/`.

---

## Build & Run (Code Composer Studio)
1. Open **CCS**.
2. Import the project:
   - `File → Import… → Code Composer Studio → CCS Projects`
   - Select the repo/project directory.
3. Build:
   - `Project → Clean`
   - `Project → Build`
4. Flash and run:
   - Click **Debug**
   - Click **Resume** (green play)

---

## Quick Hardware Test Plan (Recommended)
### Test 1 — Display
Temporarily add this after `Display_Init()` in `src/main.c`:
```c
Display_FillRect(0, 0, 128, 128, 0xF800); // solid red
while (1);
```
If the screen turns red, your TFT driver + wiring are correct.

### Test 2 — Joystick
Run the program and verify:
- cursor moves with joystick directions
- pressing joystick selects a cell

### Test 3 — Full Game
- Use the menu to select PvP or PvC
- Place moves until win/tie
- Confirm sound effects play at the correct times

---

## Main Execution Flow (Talk‑Through)
`main.c`  
→ initializes drivers (`Display_Init`, `Joystick_Init`, `Buzzer_Init`)  
→ `GameManager_Init`  
→ loop: `GameManager_Tick`  
→ read input: `Joystick_ReadAction`  
→ update game state: `TTT_ApplyMove` / `AI_ChooseMove`  
→ render: `UI_RenderGame`  
→ draw primitives: `Display_FillRect/DrawLine/DrawPixel`

---

## Notes
- Keep only one display stack in the build (this project uses the `Display_*` primitives).
- Game logic (`src/game/`) has **no** hardware dependencies by design.
