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
- `main.c`: init + main loop/state machine
- `game.c/.h`: board state, moves, win/tie checks
- `ai.c/.h`: computer move selection (simple strategy)
- `display.c/.h`: draw board/cursor/status
- `input.c/.h`: joystick reading + debouncing
- `buzzer.c/.h`: tones/melodies
