
#include "display.h"

/*
  TODO (LCD DRIVER):
  Replace these two with your lab driver calls:
   - TFT init
   - pixel draw

  Once pixel works, FillRect/DrawLine will work too.
*/

static void TFT_Driver_Init(void)
{
    // TODO: call your lab’s init (Crystalfontz128x128_Init / GrLib init / etc.)
}

static void TFT_Driver_Pixel(uint8_t x, uint8_t y, uint16_t c)
{
    // TODO: call your lab’s pixel draw
    (void)x; (void)y; (void)c;
}

void Display_Init(void)
{
    TFT_Driver_Init();
    Display_FillRect(0,0,128,128,0x0000);
}

void Display_DrawPixel(uint8_t x, uint8_t y, uint16_t rgb565)
{
    TFT_Driver_Pixel(x,y,rgb565);
}

void Display_FillRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint16_t c)
{
    for (uint8_t yy=y; yy<(uint8_t)(y+h); yy++)
        for (uint8_t xx=x; xx<(uint8_t)(x+w); xx++)
            TFT_Driver_Pixel(xx,yy,c);
}

// Bresenham
void Display_DrawLine(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint16_t c)
{
    int dx  =  (x1 > x0) ? (x1 - x0) : (x0 - x1);
    int dy  = -((y1 > y0) ? (y1 - y0) : (y0 - y1));
    int sx  = (x0 < x1) ? 1 : -1;
    int sy  = (y0 < y1) ? 1 : -1;
    int err = dx + dy;

    while (1) {
        TFT_Driver_Pixel(x0,y0,c);
        if (x0==x1 && y0==y1) break;
        int e2 = 2*err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}
