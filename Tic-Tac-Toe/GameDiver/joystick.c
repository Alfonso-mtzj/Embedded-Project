#include "msp430fr6989.h"
#include "joystick.h"

int joyX = 0;
int joyY = 0;

void joystick_init(void)
{
    // X axis A10 / P9.2
    P9SEL1 |= BIT2; P9SEL0 |= BIT2;
    // Y axis A4 / P8.7
    P8SEL1 |= BIT7; P8SEL0 |= BIT7;
    // Select button P3.1
    P3DIR &= ~But3; P3REN |= But3; P3OUT |= But3;

    ADC12CTL0 |= ADC12ON;
    ADC12CTL0 &= ~ADC12ENC;
    ADC12CTL0 |= ADC12SHT0_2 | ADC12MSC;
    ADC12CTL1  = ADC12SHS_0 | ADC12SHP | ADC12DIV_0 | ADC12SSEL_0 | ADC12CONSEQ_1;
    ADC12CTL2  = ADC12RES_2;
    ADC12CTL3 |= ADC12CSTARTADD_0;
    ADC12MCTL0 = ADC12VRSEL_0 | ADC12INCH_10;
    ADC12MCTL1 = ADC12VRSEL_0 | ADC12INCH_4 | ADC12EOS;
    ADC12CTL0 |= ADC12ENC;
}

void joystick_read(void)
{
    ADC12CTL0 |= ADC12SC;
    while(ADC12CTL1 & ADC12BUSY);
    joyX = ADC12MEM0;
    joyY = ADC12MEM1;
}

int readJoystick(void)
{
    joystick_read();
    if((P3IN & But3) == 0) return SELECT;
    if(joyX > 3500) return RIGHT;
    if(joyX < 500)  return LEFT;
    if(joyY > 3500) return UP;
    if(joyY < 500)  return DOWN;
    return CENTER;
}

void movecursor(int *row, int *col)
{
    int dir = readJoystick();
    if(dir == RIGHT && *col < 2) { buzzer_move(); (*col)++; __delay_cycles(400000); }
    if(dir == LEFT  && *col > 0) { buzzer_move(); (*col)--; __delay_cycles(400000); }
    if(dir == UP    && *row > 0) { buzzer_move(); (*row)--; __delay_cycles(400000); }
    if(dir == DOWN  && *row < 2) { buzzer_move(); (*row)++; __delay_cycles(400000); }
}

void selectOptions(int *opt, int maxOpt)
{
    int dir = readJoystick();
    if(dir == UP   && *opt > 0)      { (*opt)--; __delay_cycles(350000); }
    if(dir == DOWN && *opt < maxOpt) { (*opt)++; __delay_cycles(350000); }
}

int menuSelected(void)
{
    if(readJoystick() == SELECT)
    {
        buzzer_select();
        while((P3IN & But3) == 0);
        __delay_cycles(200000);
        return 1;
    }
    return 0;
}

void checkBoosterS2Release(void)
{
    while((P3IN & But3) == 0);
    __delay_cycles(200000);
}
