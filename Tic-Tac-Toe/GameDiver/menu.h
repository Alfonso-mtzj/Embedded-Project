#ifndef MENU_H
#define MENU_H

#include "display.h"
#include "joystick.h"

#define OPT_PVP    0
#define OPT_PVC    1
#define OPT_DUAL   2

#define DUAL_PVP   10
#define DUAL_PVC   11
#define DUAL_MIXED 12

int  run_main_menu(Graphics_Context *ctx);
int  run_dual_menu(Graphics_Context *ctx);

#endif
