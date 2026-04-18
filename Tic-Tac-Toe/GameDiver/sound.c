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
    buzzer_start(30530, 400); buzzer_wait();
    buzzer_start(24240, 500); buzzer_wait();
    buzzer_start(20400, 600); buzzer_wait();
    buzzer_start(15260, 900); buzzer_wait();
    buzzer_stop();
}

void buzzer_win(void)
{
    buzzer_start(20408, 200); buzzer_wait();
    buzzer_start(15267, 200); buzzer_wait();
    buzzer_start(12121, 400); buzzer_wait();
    buzzer_stop();
}

void buzzer_lose(void)
{
    buzzer_start(30534, 200); buzzer_wait();
    buzzer_start(36364, 200); buzzer_wait();
    buzzer_start(40000, 400); buzzer_wait();
    buzzer_stop();
}

void buzzer_tie(void)
{
    buzzer_start(24242, 150); buzzer_wait();
    buzzer_start(24242, 150); buzzer_wait();
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
