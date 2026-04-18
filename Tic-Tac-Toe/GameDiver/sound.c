#include "msp430fr6989.h"
#include "sound.h"

volatile int ticks = 0;

void buzzer_init(void)
{
    P2DIR |= BUZZ_PIN;
    P2OUT &= ~BUZZ_PIN;
}

void buzzer_start(int period, int duration)
{
    ticks = duration;
    TA0CCR0  = period;
    TA0CCTL0 = CCIE;
    TA0CTL   = TASSEL__SMCLK | MC__UP | TACLR;
}

void buzzer_stop(void)
{
    TA0CTL   = MC__STOP;
    TA0CCTL0 &= ~CCIE;
    P2OUT    &= ~BUZZ_PIN;
}

void buzzer_wait(void)
{
    while(ticks > 0);
}

void buzzer_welcome(void)
{
    // Mortal Kombat Theme - The Immortals (1994)
    buzzer_start(48484, 36); buzzer_wait(); // E4
    buzzer_start(48484, 36); buzzer_wait(); // E4
    buzzer_start(48484, 36); buzzer_wait(); // E4
    buzzer_start(48484, 72); buzzer_wait(); // E4
    buzzer_start(40816, 43); buzzer_wait(); // G4
    buzzer_start(48484, 36); buzzer_wait(); // E4
    buzzer_start(54421, 64); buzzer_wait(); // D4

    buzzer_start(48484, 36); buzzer_wait(); // E4
    buzzer_start(48484, 36); buzzer_wait(); // E4
    buzzer_start(48484, 36); buzzer_wait(); // E4
    buzzer_start(48484, 72); buzzer_wait(); // E4
    buzzer_start(36363, 291); buzzer_wait(); // A4 (held)

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

    // Rising power riff
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
    buzzer_start(20408, 345); buzzer_wait(); // G5

    buzzer_stop();
}

void buzzer_win(void)
{
    // Victory fanfare - ascending run with held high note
    buzzer_start(48484, 26); buzzer_wait(); // E4
    buzzer_start(40816, 31); buzzer_wait(); // G4
    buzzer_start(36363, 35); buzzer_wait(); // A4
    buzzer_start(32388, 39); buzzer_wait(); // B4
    buzzer_start(24279, 105); buzzer_wait(); // E5
    buzzer_start(24279, 52); buzzer_wait();  // E5
    buzzer_start(20408, 235); buzzer_wait(); // G5 held
    buzzer_stop();
}

void buzzer_lose(void)
{
    // Sad descending - game over feel
    buzzer_start(32388, 98); buzzer_wait();  // B4
    buzzer_start(36363, 88); buzzer_wait();  // A4
    buzzer_start(40816, 78); buzzer_wait();  // G4
    buzzer_start(48484, 132); buzzer_wait(); // E4 held
    buzzer_stop();
}

void buzzer_tie(void)
{
    // Price is Right losing horn - wah wah wah wahhh
    buzzer_start(34334, 69); buzzer_wait();  // Bb4
    buzzer_start(36363, 66); buzzer_wait();  // A4
    buzzer_start(38554, 62); buzzer_wait();  // Ab4
    buzzer_start(40816, 156); buzzer_wait(); // G4 held sad
    buzzer_stop();
}

void buzzer_error(void)
{
    buzzer_start(45000, 80); buzzer_wait();
    buzzer_stop();
}

void buzzer_move(void)
{
    buzzer_start(20408, 30); buzzer_wait();
    buzzer_stop();
}

void buzzer_select(void)
{
    buzzer_start(20408, 60); buzzer_wait();
    buzzer_stop();
}

#pragma vector = TIMER0_A0_VECTOR
__interrupt void Timer_ISR(void)
{
    P2OUT ^= BUZZ_PIN;
    ticks--;
    if(ticks <= 0) buzzer_stop();
}
