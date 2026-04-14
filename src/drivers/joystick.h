#ifndef JOYSTICK_H
#define JOYSTICK_H

#include <stdint.h>

typedef enum {
    JOY_NONE = 0,
    JOY_UP,
    JOY_DOWN,
    JOY_LEFT,
    JOY_RIGHT,
    JOY_SELECT,        // joystick press
    JOY_SWITCH_GAME,   // B1 button
    JOY_RESET_GAME     // optional (not wired by default)
} JoyAction;

void Joystick_Init(void);
JoyAction Joystick_ReadAction(void);

#endif
