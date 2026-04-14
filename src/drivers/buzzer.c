
#include "buzzer.h"
#include <msp430.h>

// TODO (PIN MAP): buzzer is J4.40 on EDUMKII; set correct port/bit
#define BUZZ_DIR P2DIR
#define BUZZ_OUT P2OUT
#define BUZZ_BIT BIT0

static void delay_cycles(uint32_t c){ while(c--) __no_operation(); }

void Buzzer_Init(void)
{
    BUZZ_DIR |= BUZZ_BIT;
    BUZZ_OUT &= ~BUZZ_BIT;
}

void Buzzer_BeepMs(uint16_t ms)
{
    for (uint16_t t=0; t<ms; t++) {
        for (uint16_t k=0; k<150; k++) {
            BUZZ_OUT ^= BUZZ_BIT;
            delay_cycles(200);
        }
    }
    BUZZ_OUT &= ~BUZZ_BIT;
}
