
#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>

void Display_Init(void);
void Display_DrawPixel(uint8_t x, uint8_t y, uint16_t rgb565);
void Display_FillRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint16_t rgb565);
void Display_DrawLine(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint16_t rgb565);

#endif
