// main.c
// This is the starting point of the whole program.
// It initializes all the hardware, plays the startup sound,
// shows the welcome screen, and then runs the menu in a loop
// so the player can keep playing after each game ends.

#include "sound.h"
#include "hardware.h"
#include "game.h"
#include "menu.h"
#include <stdlib.h>

// these variables live in game.c but we need them here too
extern volatile int activeBoard;  // which board is showing (1=Game A, 2=Game B)
extern volatile int needsRedraw;  // tells the game loop to redraw the screen
extern volatile int exitToMenu;   // tells the game loop to stop and go back to menu

// this flag tells the interrupt whether we are in dual game mode or not
// we only want S1 to switch boards when playing two games at once
volatile int dualMode = 0;

// =============================================================================
// PORT 1 Interrupt - handles S1 and S2 button presses
// S1 (P1.1) = switch between Game A and Game B (only works in dual mode)
// S2 (P1.2) = exit current game and go back to the main menu
// =============================================================================
#pragma vector = PORT1_VECTOR
__interrupt void Button_ISR(void)
{
    if(P1IFG & BIT1)
    {
        if(dualMode)
        {
            // toggle between game 1 and game 2
            activeBoard = (activeBoard == 1) ? 2 : 1;
            needsRedraw = 1;
        }
        P1IFG &= ~BIT1;  // clear the interrupt flag
    }
    if(P1IFG & BIT2)
    {
        exitToMenu = 1;      // signal the game loop to exit
        P1IFG &= ~BIT2;
    }
}

// =============================================================================
// Main Function
// =============================================================================
void main(void)
{
    Graphics_Context ctx;  // holds display settings used by all draw functions

    // --- Hardware Setup ---
    hw_init();                  // stop watchdog timer, unlock GPIO pins
    Initialize_Clock_System();  // set clock to 16 MHz
    display_init(&ctx);         // set up the LCD screen
    joystick_init();            // configure ADC for joystick X/Y axes
    buzzer_init();              // set up buzzer pin on P2.7

    srand(TA0R);  // seed random number generator using timer value

    // --- Button Setup (S1 and S2 on P1.1 and P1.2) ---
    P1DIR &= ~(BIT1 | BIT2);  // set pins as input
    P1REN |=  (BIT1 | BIT2);  // enable internal pull resistors
    P1OUT |=  (BIT1 | BIT2);  // pull up so button press reads as 0
    P1IES |=  (BIT1 | BIT2);  // trigger interrupt on falling edge (press)
    P1IFG &= ~(BIT1 | BIT2);  // clear any existing interrupt flags
    P1IE  |=  (BIT1 | BIT2);  // enable interrupts for both buttons

    __enable_interrupt();  // globally enable interrupts

    // --- Welcome Screen ---
    showWelcomeScreen(&ctx);
    buzzer_welcome();  // play the Mortal Kombat theme

    // wait for player to press S1 before going to the menu
    while((P1IN & BIT1) != 0);  // wait for press
    while((P1IN & BIT1) == 0);  // wait for release
    __delay_cycles(200000);      // small debounce delay

    int choice;

    // --- Main Game Loop ---
    // keeps running so the player can play again after each game
    while(1)
    {
        dualMode = 0;                    // reset dual mode flag before showing menu
        choice = run_main_menu(&ctx);    // show menu and get player's choice

        if(choice == OPT_PVP)
        {
            dualMode = 0;
            run_pvp(&ctx);               // single board, two players take turns
        }
        else if(choice == OPT_PVC)
        {
            dualMode = 0;
            run_pvc(&ctx);               // single board, player vs computer
        }
        else if(choice == DUAL_PVP)
        {
            dualMode = 1;                // S1 will now switch between boards
            run_dual_pvp(&ctx);          // two boards, both player vs player
        }
        else if(choice == DUAL_PVC)
        {
            dualMode = 1;
            run_dual_pvc(&ctx);          // two boards, both player vs CPU
        }
        else if(choice == DUAL_MIXED)
        {
            dualMode = 1;
            run_dual_mixed(&ctx);        // two boards, one PvP and one PvC
        }
    }
}
