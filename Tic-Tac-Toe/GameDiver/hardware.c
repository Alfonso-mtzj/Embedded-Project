#include "msp430fr6989.h"
#include "hardware.h"

void hw_init(void)
{
    WDTCTL   = WDTPW | WDTHOLD;
    PM5CTL0 &= ~LOCKLPM5;
}

void booster_s2_init(void)
{
    P3DIR &= ~But3;
    P3REN |=  But3;
    P3OUT |=  But3;
}
