#include <LcdDriver/lower_driver.h>
#include "msp430fr6989.h"
#include "Grlib/grlib/grlib.h"
#include <stdint.h>

// Pin definitions matching EDUMKII boosterpack on MSP430FR6989
// CLK  = P1.4
// MOSI = P1.6
// CS   = P2.5
// DC   = P2.3
// RST  = P9.4

#define CLK_HI   P1OUT |=  BIT4
#define CLK_LO   P1OUT &= ~BIT4
#define MOSI_HI  P1OUT |=  BIT6
#define MOSI_LO  P1OUT &= ~BIT6
#define CS_HI    P2OUT |=  BIT5
#define CS_LO    P2OUT &= ~BIT5
#define DC_HI    P2OUT |=  BIT3
#define DC_LO    P2OUT &= ~BIT3
#define RST_HI   P9OUT |=  BIT4
#define RST_LO   P9OUT &= ~BIT4

static void spi_send_byte(uint8_t b)
{
    int i;
    for(i = 7; i >= 0; i--)
    {
        CLK_LO;
        if(b & (1 << i)) MOSI_HI;
        else              MOSI_LO;
        CLK_HI;
    }
    CLK_LO;
}

void HAL_LCD_PortInit(void)
{
    // Set CLK, MOSI as GPIO output (bit-bang, not eUSCI)
    P1SEL0 &= ~(BIT4 | BIT6);
    P1SEL1 &= ~(BIT4 | BIT6);
    P1DIR  |=  (BIT4 | BIT6);
    P1OUT  &= ~(BIT4 | BIT6);

    // CS, DC as output
    P2DIR |= BIT5 | BIT3;
    CS_HI;
    DC_LO;

    // RST as output
    P9DIR |= BIT4;
    RST_HI;

    // Backlight on
    P2DIR |= BIT6;
    P2OUT |= BIT6;
}

void HAL_LCD_SpiInit(void)
{
    // Nothing needed for bit-bang SPI
    CS_LO;
    CLK_LO;
}

void HAL_LCD_writeCommand(uint8_t command)
{
    DC_LO;
    CS_LO;
    spi_send_byte(command);
}

void HAL_LCD_writeData(uint8_t data)
{
    DC_HI;
    CS_LO;
    spi_send_byte(data);
}

