#include "sound.h"
#include "hardware.h"
#include "game.h"
#include "menu.h"
#include <stdlib.h>

extern volatile int activeBoard;
extern volatile int needsRedraw;
extern volatile int exitToMenu;

// tracks whether we are in dual game mode
volatile int dualMode = 0;

#pragma vector = PORT1_VECTOR
__interrupt void Button_ISR(void)
{
    if(P1IFG & BIT1)
    {
        if(dualMode)
        {
            // Only switch boards in dual mode
            activeBoard = (activeBoard == 1) ? 2 : 1;
            needsRedraw = 1;
        }
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

    P1DIR &= ~(BIT1 | BIT2);
    P1REN |=  (BIT1 | BIT2);
    P1OUT |=  (BIT1 | BIT2);
    P1IES |=  (BIT1 | BIT2);
    P1IFG &= ~(BIT1 | BIT2);
    P1IE  |=  (BIT1 | BIT2);

    __enable_interrupt();

    showWelcomeScreen(&ctx);
    buzzer_welcome();

    // Wait for S1 to continue past welcome screen
    while((P1IN & BIT1) != 0);
    while((P1IN & BIT1) == 0);
    __delay_cycles(200000);

    int choice;

    while(1)
    {
        dualMode = 0;   // reset before menu
        choice = run_main_menu(&ctx);

        if(choice == OPT_PVP)
        {
            dualMode = 0;
            run_pvp(&ctx);
        }
        else if(choice == OPT_PVC)
        {
            dualMode = 0;
            run_pvc(&ctx);
        }
        else if(choice == DUAL_PVP)
        {
            dualMode = 1;   // enable S1 switching
            run_dual_pvp(&ctx);
        }
        else if(choice == DUAL_PVC)
        {
            dualMode = 1;
            run_dual_pvc(&ctx);
        }
        else if(choice == DUAL_MIXED)
        {
            dualMode = 1;
            run_dual_mixed(&ctx);
        }
    }
}
