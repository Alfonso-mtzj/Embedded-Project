
#include "sfx.h"
#include "../drivers/buzzer.h"

void SFX_Move(void)       { Buzzer_BeepMs(40); }
void SFX_Error(void)      { Buzzer_BeepMs(200); }
void SFX_Win(void)        { Buzzer_BeepMs(120); Buzzer_BeepMs(120); }
void SFX_Tie(void)        { Buzzer_BeepMs(80); }
void SFX_SwitchGame(void) { Buzzer_BeepMs(60); }
