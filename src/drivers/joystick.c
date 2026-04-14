#include "joystick.h"
#include <msp430.h>

/*
  BOOSTXL-EDUMKII joystick pins from your photo:
    HOR(X): J1.2   (analog)
    VER(Y): J3.26  (analog)
    SEL:    J1.5   (digital)
  BoosterPack B1 button is on the right side (J4.32/J4.33 area).
  TODO: confirm port/bit mapping for SEL and B1 from your lab pin map.
*/

// ---------- TODO (PIN MAP): set these correctly ----------
#define SEL_DIR   P4DIR
#define SEL_REN   P4REN
#define SEL_OUT   P4OUT
#define SEL_IN    P4IN
#define SEL_BIT   BIT1

#define B1_DIR    P3DIR
#define B1_REN    P3REN
#define B1_OUT    P3OUT
#define B1_IN     P3IN
#define B1_BIT    BIT0
// --------------------------------------------------------

#define JOY_LOW   800
#define JOY_HIGH  3200

// TODO: confirm ADC12INCH values for FR6989 with EDUMKII wiring
#define JOY_X_CHANNEL ADC12INCH_10
#define JOY_Y_CHANNEL ADC12INCH_11

static uint16_t cooldown = 0;
#define COOLDOWN 2500

static void adc_init(void)
{
    ADC12CTL0 &= ~ADC12ENC;
    ADC12CTL0 = ADC12SHT0_2 | ADC12ON;
    ADC12CTL1 = ADC12SHP;
    ADC12CTL2 = ADC12RES_2;
    ADC12CTL0 |= ADC12ENC;
}

static uint16_t adc_read(uint16_t inch)
{
    ADC12CTL0 &= ~ADC12ENC;
    ADC12MCTL0 = inch;
    ADC12CTL0 |= ADC12ENC;

    ADC12CTL0 |= ADC12SC;
    while (ADC12CTL1 & ADC12BUSY);
    return ADC12MEM0;
}

void Joystick_Init(void)
{
    adc_init();

    // SEL input pull-up
    SEL_DIR &= ~SEL_BIT;
    SEL_REN |=  SEL_BIT;
    SEL_OUT |=  SEL_BIT;

    // B1 input pull-up
    B1_DIR &= ~B1_BIT;
    B1_REN |=  B1_BIT;
    B1_OUT |=  B1_BIT;
}

JoyAction Joystick_ReadAction(void)
{
    if (cooldown) { cooldown--; return JOY_NONE; }

    if ((B1_IN & B1_BIT) == 0) {
        cooldown = COOLDOWN;
        return JOY_SWITCH_GAME;
    }

    if ((SEL_IN & SEL_BIT) == 0) {
        cooldown = COOLDOWN;
        return JOY_SELECT;
    }

    uint16_t x = adc_read(JOY_X_CHANNEL);
    uint16_t y = adc_read(JOY_Y_CHANNEL);

    if (x < JOY_LOW)  { cooldown = COOLDOWN; return JOY_LEFT; }
    if (x > JOY_HIGH) { cooldown = COOLDOWN; return JOY_RIGHT; }
    if (y < JOY_LOW)  { cooldown = COOLDOWN; return JOY_DOWN; }
    if (y > JOY_HIGH) { cooldown = COOLDOWN; return JOY_UP; }

    return JOY_NONE;
}
