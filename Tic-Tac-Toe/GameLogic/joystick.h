// joystick.h
// Defines direction codes and declares all joystick functions.

#ifndef JOYSTICK_H
#define JOYSTICK_H

#include "sound.h"
#include "hardware.h"

// return values from readJoystick()
#define CENTER 0
#define RIGHT  1
#define LEFT   2
#define UP     3
#define DOWN   4
#define SELECT 5  // joystick button pushed in

// ADC results shared across files
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
