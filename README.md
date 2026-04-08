# Embedded-Project

# Dual Tic‑Tac‑Toe (MSP430FR6989 / CCS)

## Summary
Tic‑Tac‑Toe on the **MSP430FR6989** (Code Composer Studio) controlled by the **joystick** and displayed on the **pixel display**. Uses the **buzzer** for feedback. Supports **PvP**, **PvC**, and **two games at once** (Game A and Game B) with switching mid‑game.

## Features
- **Modes:** Player vs Player, Player vs Computer  
- **Dual-game:** Two independent boards (A/B); switch anytime without losing progress  
- **Audio:** beep on move, error tone on invalid move, win/tie sound  

## Controls (typical mapping)
- Joystick **Up/Down/Left/Right:** move cursor
- **Select** (joystick press or button): place X/O
- **Switch Game** (button or long-press): toggle Game A ↔ Game B
- **Reset** (optional button/menu): restart current game

## Components Used
- **Joystick:** navigation + selection + switching games  
- **Pixel display:** board, cursor, status (game A/B, turn, win/tie)  
- **Buzzer:** move/error/win/tie tones  

## Code Organization (high-level)
- `src/main.c`: init + main loop / top-level state machine
- `src/app/`: dual-game + mode manager (PvP/PvC, Game A/B switching)
- `src/game/`: tic‑tac‑toe rules (board, moves, win/tie) + AI (hardware-free)
- `src/drivers/`: hardware drivers (joystick, pixel display, buzzer)
- `src/ui/`: rendering (draw board/cursor/status using the display driver)
- `src/audio/`: sound effects (move/error/win/tie/switch patterns)
- `src/utils/`: helpers (debounce, timing, small utilities)
- `docs/`: wiring, pinout, control mapping
- `test/`: optional tests for `src/game/`
- `tools/`: optional scripts/helpers
