// sound.h
// Defines the buzzer pin and declares all sound functions.

#ifndef SOUND_H
#define SOUND_H

#include "msp430fr6989.h"

#define BUZZ_PIN BIT7  // buzzer connected to P2.7

// buzzer control
void buzzer_init(void);
void buzzer_start(int period, int duration);
void buzzer_stop(void);
void buzzer_wait(void);

// game sounds
void buzzer_welcome(void);  // startup theme (Mortal Kombat)
void buzzer_win(void);      // ascending victory fanfare
void buzzer_lose(void);     // descending game over sound
void buzzer_tie(void);      // wah wah wah losing horn
void buzzer_error(void);    // short buzz for invalid move
void buzzer_move(void);     // click when cursor moves
void buzzer_select(void);   // beep when menu item selected

#endif
