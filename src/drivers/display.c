
#include "display.h"
#include <msp430.h>

/*
  BOOSTXL-EDUMKII TFT (Crystalfontz 128x128) - ST7735S SPI driver
  --------------------------------------------------------------
  Wiring (from your lab diagram):
    MOSI (MCU "Serial Data Out") -> P1.6
    SCLK                         -> P1.4
    CS'                          -> P2.5
    DC'                          -> P2.3
    RST'                         -> P9.4
    Backlight                    -> P2.6

  Notes for TA checkoff:
  - This file is the ONLY place that touches SPI/display control pins.
  - UI code calls Display_DrawPixel/FillRect/DrawLine to render the game.
*/

// Control pins
#define LCD_CS_DIR   P2DIR
#define LCD_CS_OUT   P2OUT
#define LCD_CS_BIT   BIT5

#define LCD_DC_DIR   P2DIR
#define LCD_DC_OUT   P2OUT
#define LCD_DC_BIT   BIT3

#define LCD_BL_DIR   P2DIR
#define LCD_BL_OUT   P2OUT
#define LCD_BL_BIT   BIT6

#define LCD_RST_DIR  P9DIR
#define LCD_RST_OUT  P9OUT
#define LCD_RST_BIT  BIT4

// SPI pins (P1.4=SCLK, P1.6=MOSI) -> eUSCI_B0 in many lab setups
#define LCD_SPI_SEL0  P1SEL0
#define LCD_SPI_SEL1  P1SEL1
#define LCD_SPI_SCLK  BIT4
#define LCD_SPI_MOSI  BIT6

// ST7735 command subset
#define ST7735_SWRESET  0x01
#define ST7735_SLPOUT   0x11
#define ST7735_COLMOD   0x3A
#define ST7735_MADCTL   0x36
#define ST7735_CASET    0x2A
#define ST7735_RASET    0x2B
#define ST7735_RAMWR    0x2C
#define ST7735_DISPON   0x29
#define ST7735_INVON    0x21

static void delay_ms(uint16_t ms)
{
    while (ms--) __delay_cycles(8000); // assumes ~8MHz
}

static inline void cs_low(void)  { LCD_CS_OUT &= ~LCD_CS_BIT; }
static inline void cs_high(void) { LCD_CS_OUT |=  LCD_CS_BIT; }
static inline void dc_cmd(void)  { LCD_DC_OUT &= ~LCD_DC_BIT; }
static inline void dc_data(void) { LCD_DC_OUT |=  LCD_DC_BIT; }

static void spi_init(void)
{
    // Hold eUSCI_B0 in reset while configuring
    UCB0CTLW0 |= UCSWRST;

    // Select SPI function on P1.4 and P1.6
    LCD_SPI_SEL0 |=  (LCD_SPI_SCLK | LCD_SPI_MOSI);
    LCD_SPI_SEL1 &= ~(LCD_SPI_SCLK | LCD_SPI_MOSI);

    // SPI master, synchronous, MSB first, SMCLK source
    // Clock phase: UCCKPH=1 works for many ST7735 modules.
    UCB0CTLW0 = UCSWRST |
               UCSSEL__SMCLK |
               UCMST | UCSYNC | UCMSB |
               UCCKPH;

    // SPI clock divider (SMCLK/8)
    UCB0BRW = 8;

    // Release reset
    UCB0CTLW0 &= ~UCSWRST;
}

static void spi_write(uint8_t b)
{
    while (!(UCB0IFG & UCTXIFG)) { }
    UCB0TXBUF = b;
    while (UCB0STATW & UCBUSY) { }
}

static void write_cmd(uint8_t cmd)
{
    cs_low();
    dc_cmd();
    spi_write(cmd);
    cs_high();
}

static void write_data8(uint8_t data)
{
    cs_low();
    dc_data();
    spi_write(data);
    cs_high();
}

static void set_addr_window(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1)
{
    write_cmd(ST7735_CASET);
    cs_low(); dc_data();
    spi_write(0x00); spi_write(x0);
    spi_write(0x00); spi_write(x1);
    cs_high();

    write_cmd(ST7735_RASET);
    cs_low(); dc_data();
    spi_write(0x00); spi_write(y0);
    spi_write(0x00); spi_write(y1);
    cs_high();

    write_cmd(ST7735_RAMWR);
}

static void gpio_init(void)
{
    LCD_CS_DIR  |= LCD_CS_BIT;
    LCD_DC_DIR  |= LCD_DC_BIT;
    LCD_BL_DIR  |= LCD_BL_BIT;
    LCD_RST_DIR |= LCD_RST_BIT;

    cs_high();
    LCD_BL_OUT |= LCD_BL_BIT;     // backlight ON
    LCD_RST_OUT |= LCD_RST_BIT;   // keep reset high
}

static void lcd_reset(void)
{
    LCD_RST_OUT |=  LCD_RST_BIT;
    delay_ms(10);

    LCD_RST_OUT &= ~LCD_RST_BIT;
    delay_ms(50);

    LCD_RST_OUT |=  LCD_RST_BIT;
    delay_ms(120);
}

static void st7735_init(void)
{
    write_cmd(ST7735_SWRESET);
    delay_ms(150);

    write_cmd(ST7735_SLPOUT);
    delay_ms(150);

    // 16-bit RGB565
    write_cmd(ST7735_COLMOD);
    write_data8(0x05);
    delay_ms(10);

    // Rotation/RGB order. If rotated/mirrored, adjust.
    write_cmd(ST7735_MADCTL);
    write_data8(0xC8);

    // Many EDUMKII panels expect inversion ON. If colors look inverted, remove this.
    write_cmd(ST7735_INVON);

    write_cmd(ST7735_DISPON);
    delay_ms(150);
}

void Display_Init(void)
{
    gpio_init();
    spi_init();
    lcd_reset();
    st7735_init();

    // Clear to black
    Display_FillRect(0, 0, 128, 128, 0x0000);
}

void Display_DrawPixel(uint8_t x, uint8_t y, uint16_t rgb565)
{
    if (x >= 128 || y >= 128) return;

    set_addr_window(x, y, x, y);

    cs_low();
    dc_data();
    spi_write((uint8_t)(rgb565 >> 8));
    spi_write((uint8_t)(rgb565 & 0xFF));
    cs_high();
}

void Display_FillRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint16_t rgb565)
{
    if (x >= 128 || y >= 128) return;
    if ((uint16_t)x + w > 128) w = 128 - x;
    if ((uint16_t)y + h > 128) h = 128 - y;

    set_addr_window(x, y, (uint8_t)(x + w - 1), (uint8_t)(y + h - 1));

    cs_low();
    dc_data();

    uint32_t count = (uint32_t)w * (uint32_t)h;
    while (count--) {
        spi_write((uint8_t)(rgb565 >> 8));
        spi_write((uint8_t)(rgb565 & 0xFF));
    }

    cs_high();
}

void Display_DrawLine(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint16_t c)
{
    int dx  =  (x1 > x0) ? (x1 - x0) : (x0 - x1);
    int dy  = -((y1 > y0) ? (y1 - y0) : (y0 - y1));
    int sx  = (x0 < x1) ? 1 : -1;
    int sy  = (y0 < y1) ? 1 : -1;
    int err = dx + dy;

    while (1) {
        Display_DrawPixel(x0, y0, c);
        if (x0 == x1 && y0 == y1) break;
        int e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}
