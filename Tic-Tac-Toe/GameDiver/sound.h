#ifndef SOUND_H
#define SOUND_H

#include "msp430fr6989.h"

#define BUZZ_PIN BIT7   // P2.7

void buzzer_init(void);
void buzzer_start(int period, int duration);
void buzzer_stop(void);
void buzzer_wait(void);
void buzzer_win(void);
void buzzer_lose(void);
void buzzer_tie(void);
void buzzer_error(void);
void buzzer_move(void);
void buzzer_select(void);
void buzzer_welcome(void);

#endif
