// hardware.c
// This file handles basic hardware setup.
// It stops the watchdog timer and unlocks the GPIO pins
// so the rest of the program can use them.

#include "msp430fr6989.h"
#include "hardware.h"

// =============================================================================
// Hardware Initialization
// Must be called first before anything else uses GPIO pins
// =============================================================================

// stops the watchdog timer so it doesn't reset the board
// and unlocks the GPIO pins which are locked by default on power-up
void hw_init(void)
{
    WDTCTL   = WDTPW | WDTHOLD;  // stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5;         // unlock GPIO pins
}

// sets up the booster pack S2 button (P3.1) as input with pull-up resistor
// this is the joystick push button used to place marks in the game
void booster_s2_init(void)
{
    P3DIR &= ~But3;  // set P3.1 as input
    P3REN |=  But3;  // enable pull resistor
    P3OUT |=  But3;  // pull up so button press reads as 0
}
