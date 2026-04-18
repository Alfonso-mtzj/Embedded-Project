# Tic‑Tac‑Toe (MSP430FR6989 + BOOSTXL‑EDUMKII)

This repository contains a Tic‑Tac‑Toe game built for the **MSP‑EXP430FR6989 LaunchPad** + **Educational BoosterPack MKII** (128×128 TFT, joystick, buzzer).

## Important: where the real project is
The **CCS project you should import/build is in**:

- `Tic-Tac-Toe/`

That folder contains the entry point (`main.c`), linker command file, target config, and the LCD/GrLib sources used by the project.

## How to build/run in Code Composer Studio (CCS)
1. Open **CCS**
2. Import the project:
   - `File → Import… → Code Composer Studio → CCS Projects`
   - Browse to this repo
   - Select **`Tic-Tac-Toe`** as the project to import
3. Build:
   - `Project → Clean`
   - `Project → Build`
4. Flash and run:
   - Click **Debug**
   - Click **Resume** (green play)

## Controls
- **Joystick**: move cursor (X/Y via ADC)
- **Joystick press (SEL)**: select/place
- **S1 (P1.1)**: toggles active board in dual mode
- **S2 (P1.2)**: return to main menu
- **Buzzer**: sound effects

## Code map (quick)
- `Tic-Tac-Toe/main.c` — program entry, initializes hardware, runs menu + game modes
- `Tic-Tac-Toe/game.c/.h` — game rules and state
- `Tic-Tac-Toe/menu.c/.h` — menu logic
- `Tic-Tac-Toe/display.c/.h` — screen drawing helpers (uses GrLib/LcdDriver)
- `Tic-Tac-Toe/joystick.c/.h` — joystick ADC + select button
- `Tic-Tac-Toe/sound.c/.h` — buzzer sound effects
- `Tic-Tac-Toe/hardware.c/.h` — board/peripheral setup
- `Tic-Tac-Toe/Grlib/` + `Tic-Tac-Toe/LcdDriver/` — graphics + LCD driver code
