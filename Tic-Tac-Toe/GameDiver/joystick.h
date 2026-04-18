#ifndef JOYSTICK_H
#define JOYSTICK_H

#include "sound.h"
#include "hardware.h"

#define CENTER 0
#define RIGHT  1
#define LEFT   2
#define UP     3
#define DOWN   4
#define SELECT 5

extern int joyX;
extern int joyY;

void joystick_init(void);
void joystick_read(void);
int  readJoystick(void);
void movecursor(int *row, int *col);
void selectOptions(int *opt, int maxOpt);
int  menuSelected(void);
void checkBoosterS2Release(void);

#endif
