
#ifndef JOYSTICK_H
#define JOYSTICK_H

#include <stdint.h>

// High-level actions the rest of the app uses.
typedef enum {
    JOY_NONE = 0,
    JOY_UP,
    JOY_DOWN,
    JOY_LEFT,
    JOY_RIGHT,
    JOY_SELECT,
    JOY_SWITCH_GAME,   // optional (button or long-press)
    JOY_TOGGLE_MODE    // optional (PvP <-> PvC)
} JoyAction;

void Joystick_Init(void);

// Read ONE action (already debounced / rate-limited).
JoyAction Joystick_ReadAction(void);

#endif
