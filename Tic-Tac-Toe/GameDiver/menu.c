// menu.c
// This file runs the main menu and the two-game sub-menu.
// It uses the joystick to move a selection arrow up and down
// and the joystick button to confirm the selection.
// Pressing S2 in the sub-menu goes back to the main menu.

#include "msp430fr6989.h"
#include "menu.h"
#include "sound.h"

// =============================================================================
// Main Menu
// Shows three options: Player vs Player, Player vs CPU, Two Games
// Returns the selected option number so main() knows which game to run
// =============================================================================
int run_main_menu(Graphics_Context *ctx)
{
    int sel = 0;    // currently highlighted option (0, 1, or 2)
    int prev = -1;  // tracks the last drawn selection to avoid unnecessary redraws

    while(1)
    {
        // only redraw the menu if the selection changed
        if(sel != prev)
        {
            Menu(ctx, sel);  // draws the menu with arrow at current selection
            prev = sel;
        }

        selectOptions(&sel, 2);  // move selection up or down with joystick

        if(menuSelected())  // joystick button pressed
        {
            if(sel == OPT_DUAL)
            {
                // player chose Two Games, go to the sub-menu
                int sub = run_dual_menu(ctx);
                if(sub == -1)
                {
                    // player pressed S2 to go back, reset main menu
                    sel = 0;
                    prev = -1;
                    continue;
                }
                return sub;  // return the dual mode choice
            }
            return sel;  // return PvP or PvC choice
        }
    }
}

// =============================================================================
// Two Games Sub-Menu
// Shows three dual-game options: both PvP, both PvC, or one of each
// Press S2 to go back to the main menu (returns -1)
// =============================================================================
int run_dual_menu(Graphics_Context *ctx)
{
    int sel = 0;
    int prev = -1;

    while(1)
    {
        // check if S2 was pressed to go back to main menu
        if((P1IN & BIT2) == 0)
        {
            while((P1IN & BIT2) == 0);  // wait for release
            __delay_cycles(200000);
            return -1;  // signal to go back
        }

        // redraw sub-menu only if selection changed
        if(sel != prev)
        {
            Mult_game(ctx, sel);
            prev = sel;
        }

        selectOptions(&sel, 2);

        if(menuSelected())
        {
            // return a unique number for each dual game mode
            if(sel == 0) return DUAL_PVP;    // both player vs player
            if(sel == 1) return DUAL_PVC;    // both player vs CPU
            if(sel == 2) return DUAL_MIXED;  // one of each
        }
    }
}
