# src/app/

Application logic (glue code).

Responsibilities:
- Own **two independent games** (Game A and Game B)
- Track current mode: PvP vs PvC
- Handle switching between boards mid-game
- Call into:
  - `game/` for rules + AI
  - `drivers/` for joystick/display/buzzer I/O
  - `ui/` to render
  - `audio/` for beep patterns

This is where you implement the "two games running at once" requirement.
