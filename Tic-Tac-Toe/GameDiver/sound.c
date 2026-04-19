// sound.c
// This file controls the buzzer on P2.7.
// It uses Timer0_A0 in up mode to toggle the buzzer pin at a set frequency.
// The period controls the pitch and the duration controls how long it plays.
// All sounds are made by playing sequences of notes one after another.

#include "msp430fr6989.h"
#include "sound.h"

// counts down the number of timer interrupts remaining for the current note
volatile int ticks = 0;

// =============================================================================
// Buzzer Control Functions
// =============================================================================

// sets up the buzzer pin as output and makes sure it starts off
void buzzer_init(void)
{
    P2DIR |= BUZZ_PIN;    // set P2.7 as output
    P2OUT &= ~BUZZ_PIN;   // start with buzzer off
}

// starts playing a note at the given frequency for the given duration
// period controls pitch: smaller = higher pitch (period = 16MHz / frequency)
// duration is how many timer ticks the note lasts
void buzzer_start(int period, int duration)
{
    ticks    = duration;   // set how long to play
    TA0CCR0  = period;     // set timer period (controls frequency)
    TA0CCTL0 = CCIE;       // enable timer interrupt
    TA0CTL   = TASSEL__SMCLK | MC__UP | TACLR;  // use SMCLK, count up, clear timer
}

// immediately stops the buzzer and disables the timer
void buzzer_stop(void)
{
    TA0CTL   = MC__STOP;       // stop the timer
    TA0CCTL0 &= ~CCIE;         // disable the interrupt
    P2OUT    &= ~BUZZ_PIN;     // make sure buzzer pin is low
}

// blocks until the current note finishes playing
void buzzer_wait(void)
{
    while(ticks > 0);  // wait for ISR to count down to zero
}

// =============================================================================
// Game Sounds
// Each sound is a sequence of notes played one after another
// period = 16,000,000 / frequency  (e.g. E4 at 330Hz = 48484)
// =============================================================================

// startup sound - plays the Mortal Kombat theme intro
void buzzer_welcome(void)
{
    // Mortal Kombat Theme - The Immortals (1994)
    buzzer_start(48484, 36); buzzer_wait(); // E4
    buzzer_start(48484, 36); buzzer_wait(); // E4
    buzzer_start(48484, 36); buzzer_wait(); // E4
    buzzer_start(48484, 72); buzzer_wait(); // E4 (longer)
    buzzer_start(40816, 43); buzzer_wait(); // G4
    buzzer_start(48484, 36); buzzer_wait(); // E4
    buzzer_start(54421, 64); buzzer_wait(); // D4

    buzzer_start(48484, 36); buzzer_wait(); // E4
    buzzer_start(48484, 36); buzzer_wait(); // E4
    buzzer_start(48484, 36); buzzer_wait(); // E4
    buzzer_start(48484, 72); buzzer_wait(); // E4
    buzzer_start(36363, 291); buzzer_wait(); // A4 (held note)

    buzzer_start(48484, 36); buzzer_wait(); // E4
    buzzer_start(48484, 36); buzzer_wait(); // E4
    buzzer_start(48484, 36); buzzer_wait(); // E4
    buzzer_start(48484, 72); buzzer_wait(); // E4
    buzzer_start(40816, 43); buzzer_wait(); // G4
    buzzer_start(48484, 36); buzzer_wait(); // E4
    buzzer_start(54421, 64); buzzer_wait(); // D4
    buzzer_start(30592, 115); buzzer_wait(); // C5
    buzzer_start(32388, 108); buzzer_wait(); // B4
    buzzer_start(36363, 194); buzzer_wait(); // A4

    // rising power riff at the end
    buzzer_start(48484, 72); buzzer_wait(); // E4
    buzzer_start(40816, 86); buzzer_wait(); // G4
    buzzer_start(36363, 97); buzzer_wait(); // A4
    buzzer_start(32388, 108); buzzer_wait(); // B4
    buzzer_start(24279, 290); buzzer_wait(); // E5
    buzzer_start(27257, 129); buzzer_wait(); // D5
    buzzer_start(32388, 108); buzzer_wait(); // B4

    buzzer_start(48484, 72); buzzer_wait(); // E4
    buzzer_start(40816, 86); buzzer_wait(); // G4
    buzzer_start(36363, 97); buzzer_wait(); // A4
    buzzer_start(32388, 108); buzzer_wait(); // B4
    buzzer_start(20408, 345); buzzer_wait(); // G5 (final held note)

    buzzer_stop();
}

// win sound - quick ascending notes, feels like a victory fanfare
void buzzer_win(void)
{
    buzzer_start(48484, 26); buzzer_wait(); // E4
    buzzer_start(40816, 31); buzzer_wait(); // G4
    buzzer_start(36363, 35); buzzer_wait(); // A4
    buzzer_start(32388, 39); buzzer_wait(); // B4
    buzzer_start(24279, 105); buzzer_wait(); // E5
    buzzer_start(24279, 52); buzzer_wait();  // E5
    buzzer_start(20408, 235); buzzer_wait(); // G5 held
    buzzer_stop();
}

// lose sound - slow descending notes, classic game over feel
void buzzer_lose(void)
{
    buzzer_start(32388, 98); buzzer_wait();  // B4
    buzzer_start(36363, 88); buzzer_wait();  // A4
    buzzer_start(40816, 78); buzzer_wait();  // G4
    buzzer_start(48484, 132); buzzer_wait(); // E4 (held low note)
    buzzer_stop();
}

// tie sound - "wah wah wah wahhh" descending like the Price is Right losing horn
void buzzer_tie(void)
{
    buzzer_start(34334, 69); buzzer_wait();  // Bb4
    buzzer_start(36363, 66); buzzer_wait();  // A4
    buzzer_start(38554, 62); buzzer_wait();  // Ab4
    buzzer_start(40816, 156); buzzer_wait(); // G4 (held sad note)
    buzzer_stop();
}

// short low buzz when player tries to place on a taken cell
void buzzer_error(void)
{
    buzzer_start(45000, 80); buzzer_wait();
    buzzer_stop();
}

// quick click sound when the cursor moves
void buzzer_move(void)
{
    buzzer_start(20408, 30); buzzer_wait();
    buzzer_stop();
}

// short beep when a menu option is selected
void buzzer_select(void)
{
    buzzer_start(20408, 60); buzzer_wait();
    buzzer_stop();
}

// =============================================================================
// Timer0_A0 Interrupt Service Routine
// Fires every time the timer counts up to TA0CCR0
// Toggles the buzzer pin to create sound and counts down the note duration
// =============================================================================
#pragma vector = TIMER0_A0_VECTOR
__interrupt void Timer_ISR(void)
{
    P2OUT ^= BUZZ_PIN;  // toggle buzzer pin to create the sound wave
    ticks--;            // count down the remaining duration
    if(ticks <= 0) buzzer_stop();  // stop when duration runs out
}
