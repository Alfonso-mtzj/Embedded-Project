#include "joystick.h"
#include <msp430.h>


// -------- SEL button (joystick press) ----------
#define SEL_DIR   P3DIR
#define SEL_REN   P3REN
#define SEL_OUT   P3OUT
#define SEL_IN    P3IN
#define SEL_BIT   BIT2
// ----------------------------------------------

// --------- B1 swap button (still TODO) --------
// If you don't have the mapping yet, comment this out or keep it as placeholder.
// Paste your booster button mapping and I’ll fill it exactly.
#define B1_DIR    P3DIR
#define B1_REN    P3REN
#define B1_OUT    P3OUT
#define B1_IN     P3IN
#define B1_BIT    BIT0   // TODO: verify actual B1 pin (NOT confirmed yet)
// ----------------------------------------------

#define JOY_LOW   800
#define JOY_HIGH  3200

static uint16_t cooldown = 0;
#define COOLDOWN 2500

static void adc_init_sequence_xy(void)
{
    // ---- Configure analog pins ----
    // X: A10 / P9.2
    P9SEL1 |= BIT2;
    P9SEL0 |= BIT2;

    // Y: A4 / P8.7
    P8SEL1 |= BIT7;
    P8SEL0 |= BIT7;

    // ---- ADC12_B setup ----
    ADC12CTL0 &= ~ADC12ENC;

    // ADC on + sample/hold + multiple sample/convert
    ADC12CTL0 = ADC12ON | ADC12SHT0_2 | ADC12MSC;

    // Sample timer + sequence-of-channels
    // Trigger source = ADC12SC
    ADC12CTL1 = ADC12SHP | ADC12CONSEQ_1;

    // 12-bit resolution
    ADC12CTL2 = ADC12RES_2;

    // Start address = ADC12MEM0
    ADC12CTL3 = ADC12CSTARTADD_0;

    // MEM0 = X (A10)
    ADC12MCTL0 = ADC12VRSEL_0 | ADC12INCH_10;

    // MEM1 = Y (A4), end of sequence
    ADC12MCTL1 = ADC12VRSEL_0 | ADC12INCH_4 | ADC12EOS;

    ADC12CTL0 |= ADC12ENC;
}

static void read_xy(uint16_t* x, uint16_t* y)
{
    ADC12CTL0 |= ADC12SC;
    while (ADC12CTL1 & ADC12BUSY) { }
    *x = ADC12MEM0;
    *y = ADC12MEM1;
}

void Joystick_Init(void)
{
    // SEL pull-up (active-low)
    SEL_DIR &= ~SEL_BIT;
    SEL_REN |=  SEL_BIT;
    SEL_OUT |=  SEL_BIT;

    // B1 pull-up (active-low) (verify pin!)
    B1_DIR &= ~B1_BIT;
    B1_REN |=  B1_BIT;
    B1_OUT |=  B1_BIT;

    adc_init_sequence_xy();
}

JoyAction Joystick_ReadAction(void)
{
    if (cooldown) { cooldown--; return JOY_NONE; }

    // Swap game with B1 (if mapped correctly)
    if ((B1_IN & B1_BIT) == 0) {
        cooldown = COOLDOWN;
        return JOY_SWITCH_GAME;
    }

    // Joystick press
    if ((SEL_IN & SEL_BIT) == 0) {
        cooldown = COOLDOWN;
        return JOY_SELECT;
    }

    uint16_t x, y;
    read_xy(&x, &y);

    if (x < JOY_LOW)  { cooldown = COOLDOWN; return JOY_LEFT;  }
    if (x > JOY_HIGH) { cooldown = COOLDOWN; return JOY_RIGHT; }
    if (y < JOY_LOW)  { cooldown = COOLDOWN; return JOY_DOWN;  }
    if (y > JOY_HIGH) { cooldown = COOLDOWN; return JOY_UP;    }

    return JOY_NONE;
}
