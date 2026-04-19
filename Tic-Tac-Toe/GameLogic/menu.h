// menu.h
// Defines the menu option codes and declares the menu functions.

#ifndef MENU_H
#define MENU_H

#include "display.h"
#include "joystick.h"

// main menu option indices
#define OPT_PVP    0  // Player vs Player
#define OPT_PVC    1  // Player vs CPU
#define OPT_DUAL   2  // Two Games

// dual game sub-menu return values
// using 10/11/12 so they don't conflict with the single game values 0/1
#define DUAL_PVP   10  // both boards player vs player
#define DUAL_PVC   11  // both boards player vs CPU
#define DUAL_MIXED 12  // Game A vs CPU, Game B vs Player

int run_main_menu(Graphics_Context *ctx);
int run_dual_menu(Graphics_Context *ctx);

#endif
