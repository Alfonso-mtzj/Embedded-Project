
#include "joystick.h"
#include <msp430.h>

/*
FILL THIS IN:
- Which ADC channels are joystick X and Y?
- Which GPIO pin is SELECT button?
- Optional: which GPIO pin is SWITCH GAME button?

If you tell me the pins, I can replace all of this with the exact working ADC code.
*/

// ---------- FILL IN THESE MACROS ----------
#define JOY_X_ADC_CHANNEL   (0)  // TODO: set to correct ADC channel number
#define JOY_Y_ADC_CHANNEL   (1)  // TODO: set to correct ADC channel number

#define SELECT_PORT_DIR     P1DIR   // TODO
#define SELECT_PORT_REN     P1REN   // TODO
#define SELECT_PORT_OUT     P1OUT   // TODO
#define SELECT_PORT_IN      P1IN    // TODO
#define SELECT_PIN_MASK     BIT1    // TODO: pin mask

// Optional separate button for switching games (recommended)
#define SWITCH_PORT_DIR     P1DIR   // TODO
#define SWITCH_PORT_REN     P1REN   // TODO
#define SWITCH_PORT_OUT     P1OUT   // TODO
#define SWITCH_PORT_IN      P1IN    // TODO
#define SWITCH_PIN_MASK     BIT2    // TODO: pin mask (or set to 0 if unused)
// ----------------------------------------

// Thresholds for joystick direction from ADC reading.
// These depend on your joystick + reference voltage.
// Start with these and adjust by printing values or testing.
#define ADC_CENTER_MIN  12000
#define ADC_CENTER_MAX  20000
#define ADC_LOW_THRESH   8000
#define ADC_HIGH_THRESH 24000

// Simple rate limit so holding the joystick doesn't move 100x per second.
#define REPEAT_DELAY_TICKS  4000

static uint16_t cooldown = 0;

static uint16_t adc_read_channel(uint8_t ch)
{
    (void)ch;
    // TODO: implement ADC read on MSP430FR6989 (ADC12_B).
    // For now, return a fake "center" value so code compiles.
    return 16000;
}

void Joystick_Init(void)
{
    // TODO: ADC12_B setup (clock, sample/hold, enable)
    // TODO: configure ADC input pins for X/Y

    // Select button input pull-up
    SELECT_PORT_DIR &= ~SELECT_PIN_MASK;
    SELECT_PORT_REN |=  SELECT_PIN_MASK;
    SELECT_PORT_OUT |=  SELECT_PIN_MASK; // pull-up

    // Switch button input pull-up (optional)
    if (SWITCH_PIN_MASK != 0) {
        SWITCH_PORT_DIR &= ~SWITCH_PIN_MASK;
        SWITCH_PORT_REN |=  SWITCH_PIN_MASK;
        SWITCH_PORT_OUT |=  SWITCH_PIN_MASK; // pull-up
    }
}

JoyAction Joystick_ReadAction(void)
{
    // cooldown timer to avoid too-fast repeats
    if (cooldown > 0) {
        cooldown--;
        return JOY_NONE;
    }

    // 1) buttons (active-low because of pull-up)
    if ((SELECT_PORT_IN & SELECT_PIN_MASK) == 0) {
        cooldown = REPEAT_DELAY_TICKS;
        return JOY_SELECT;
    }

    if (SWITCH_PIN_MASK != 0 && ((SWITCH_PORT_IN & SWITCH_PIN_MASK) == 0)) {
        cooldown = REPEAT_DELAY_TICKS;
        return JOY_SWITCH_GAME;
    }

    // 2) analog joystick
    uint16_t x = adc_read_channel(JOY_X_ADC_CHANNEL);
    uint16_t y = adc_read_channel(JOY_Y_ADC_CHANNEL);

    // Interpret X/Y into directional actions
    if (x < ADC_LOW_THRESH)  { cooldown = REPEAT_DELAY_TICKS; return JOY_LEFT;  }
    if (x > ADC_HIGH_THRESH) { cooldown = REPEAT_DELAY_TICKS; return JOY_RIGHT; }
    if (y < ADC_LOW_THRESH)  { cooldown = REPEAT_DELAY_TICKS; return JOY_DOWN;  }
    if (y > ADC_HIGH_THRESH) { cooldown = REPEAT_DELAY_TICKS; return JOY_UP;    }

    // No action
    return JOY_NONE;
}
