#include "msp430fr6989.h"
#include "Grlib/grlib/grlib.h"
#include "LcdDriver/lcd_driver.h"
#include "display.h"
#include "joystick.h"
#include "sound.h"
#include "hardware.h"
#include "game.h"
#include "menu.h"
#include <stdlib.h>

// Shared with game.c via extern
extern volatile int activeBoard;
extern volatile int needsRedraw;
extern volatile int exitToMenu;

// S1 toggles active board in dual mode
// S2 returns to main menu
#pragma vector = PORT1_VECTOR
__interrupt void Button_ISR(void)
{
    if(P1IFG & BIT1)
    {
        activeBoard = (activeBoard == 1) ? 2 : 1;
        needsRedraw = 1;
        P1IFG &= ~BIT1;
    }
    if(P1IFG & BIT2)
    {
        exitToMenu = 1;
        P1IFG &= ~BIT2;
    }
}

void main(void)
{
    Graphics_Context ctx;

    hw_init();
    Initialize_Clock_System();
    display_init(&ctx);
    joystick_init();
    buzzer_init();

    srand(TA0R);

    // Configure S1 (P1.1) and S2 (P1.2) with interrupts
    P1DIR &= ~(BIT1 | BIT2);
    P1REN |=  (BIT1 | BIT2);
    P1OUT |=  (BIT1 | BIT2);
    P1IES |=  (BIT1 | BIT2);
    P1IFG &= ~(BIT1 | BIT2);
    P1IE  |=  (BIT1 | BIT2);

    __enable_interrupt();

    showWelcomeScreen(&ctx);
    buzzer_welcome();

    // Wait for S1 to continue
    while((P1IN & BIT1) != 0);
    while((P1IN & BIT1) == 0);
    __delay_cycles(200000);

    int choice;

    while(1)
    {
        choice = run_main_menu(&ctx);

        if(choice == OPT_PVP)        run_pvp(&ctx);
        else if(choice == OPT_PVC)   run_pvc(&ctx);
        else if(choice == DUAL_PVP)  run_dual_pvp(&ctx);
        else if(choice == DUAL_PVC)  run_dual_pvc(&ctx);
        else if(choice == DUAL_MIXED) run_dual_mixed(&ctx);
    }
}
