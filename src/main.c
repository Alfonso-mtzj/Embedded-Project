
#include <msp430.h>
#include "app/game_manager.h"
#include "drivers/buzzer.h"
#include "drivers/display.h"
#include "drivers/joystick.h"

static void delay_ms(uint16_t ms)
{
    while (ms--) __delay_cycles(8000); // assumes ~8MHz; adjust if needed
}

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;

    // Init hardware drivers
    Display_Init();     // TFT init (you must hook to lab driver inside)
    Joystick_Init();    // ADC + SEL + B1 (you must map pins inside)
    Buzzer_Init();      // buzzer pin mapping required

    GameManager gm;
    GameManager_Init(&gm);

    while (1) {
        GameManager_Tick(&gm);
        delay_ms(25);
    }
}
