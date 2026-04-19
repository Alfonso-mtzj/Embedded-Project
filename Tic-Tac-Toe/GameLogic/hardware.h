// hardware.h
// Defines button pin names and declares hardware setup functions.

#ifndef HARDWARE_H
#define HARDWARE_H

// button pin definitions
#define But1  BIT1  // S1 on LaunchPad (P1.1)
#define But2  BIT2  // S2 on LaunchPad (P1.2)
#define But3  BIT1  // joystick push button on boosterpack (P3.1)

void hw_init(void);
void booster_s2_init(void);

#endif
