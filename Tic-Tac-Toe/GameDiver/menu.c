#include "msp430fr6989.h"
#include "menu.h"
#include "sound.h"

int run_main_menu(Graphics_Context *ctx)
{
    int sel = 0, prev = -1;

    while(1)
    {
        if(sel != prev)
        {
            Menu(ctx, sel);
            prev = sel;
        }

        selectOptions(&sel, 2);

        if(menuSelected())
        {
            if(sel == OPT_DUAL)
            {
                int sub = run_dual_menu(ctx);
                if(sub == -1) { sel = 0; prev = -1; continue; }
                return sub;
            }
            return sel;
        }
    }
}

int run_dual_menu(Graphics_Context *ctx)
{
    int sel = 0, prev = -1;

    while(1)
    {
        // S2 goes back
        if((P1IN & BIT2) == 0)
        {
            while((P1IN & BIT2) == 0);
            __delay_cycles(200000);
            return -1;
        }

        if(sel != prev)
        {
            Mult_game(ctx, sel);
            prev = sel;
        }

        selectOptions(&sel, 2);

        if(menuSelected())
        {
            if(sel == 0) return DUAL_PVP;
            if(sel == 1) return DUAL_PVC;
            if(sel == 2) return DUAL_MIXED;
        }
    }
}
