// joystick.c
// This file handles all joystick input.
// It sets up the ADC to read the X and Y axes,
// detects which direction the joystick is pushed,
// and moves the cursor or menu selection accordingly.

#include "msp430fr6989.h"
#include "joystick.h"

// stores the last ADC readings for the joystick axes
int joyX = 0;  // horizontal axis value (0-4095)
int joyY = 0;  // vertical axis value (0-4095)

// =============================================================================
// Joystick Initialization
// Configures the ADC12 module to read both joystick axes
// X axis is on channel A10 (P9.2), Y axis is on channel A4 (P8.7)
// The joystick push button is on P3.1
// =============================================================================
void joystick_init(void)
{
    // configure X axis pin as analog input (A10 on P9.2)
    P9SEL1 |= BIT2; P9SEL0 |= BIT2;

    // configure Y axis pin as analog input (A4 on P8.7)
    P8SEL1 |= BIT7; P8SEL0 |= BIT7;

    // configure joystick push button as digital input with pull-up resistor
    P3DIR &= ~But3; P3REN |= But3; P3OUT |= But3;

    // turn on ADC module
    ADC12CTL0 |= ADC12ON;
    ADC12CTL0 &= ~ADC12ENC;  // disable conversion while configuring

    // set sample time and enable multiple sample mode
    ADC12CTL0 |= ADC12SHT0_2 | ADC12MSC;

    // use ADC12SC trigger, pulse sample, MODOSC clock, sequence of channels
    ADC12CTL1  = ADC12SHS_0 | ADC12SHP | ADC12DIV_0 | ADC12SSEL_0 | ADC12CONSEQ_1;

    // 12-bit resolution, unsigned format
    ADC12CTL2  = ADC12RES_2;

    // start conversions in memory register 0
    ADC12CTL3 |= ADC12CSTARTADD_0;

    // memory register 0: read X axis (channel A10)
    ADC12MCTL0 = ADC12VRSEL_0 | ADC12INCH_10;

    // memory register 1: read Y axis (channel A4), mark as last conversion
    ADC12MCTL1 = ADC12VRSEL_0 | ADC12INCH_4 | ADC12EOS;

    // enable conversions
    ADC12CTL0 |= ADC12ENC;
}

// =============================================================================
// ADC Conversion
// Triggers a conversion and waits for it to finish
// Results are stored in joyX and joyY
// =============================================================================
void joystick_read(void)
{
    ADC12CTL0 |= ADC12SC;              // start conversion
    while(ADC12CTL1 & ADC12BUSY);     // wait until done
    joyX = ADC12MEM0;                  // read X axis result
    joyY = ADC12MEM1;                  // read Y axis result
}

// =============================================================================
// Direction Detection
// Reads the joystick and returns which direction it is pushed
// Center value is around 2048, thresholds are at 500 and 3500
// =============================================================================
int readJoystick(void)
{
    joystick_read();
    if((P3IN & But3) == 0) return SELECT;  // button pushed in
    if(joyX > 3500) return RIGHT;
    if(joyX < 500)  return LEFT;
    if(joyY > 3500) return UP;
    if(joyY < 500)  return DOWN;
    return CENTER;
}

// =============================================================================
// Cursor Movement
// Moves the game cursor one cell in the joystick direction
// The delay controls how fast the cursor moves when held
// =============================================================================
void movecursor(int *row, int *col)
{
    int dir = readJoystick();
    if(dir == RIGHT && *col < 2) { buzzer_move(); (*col)++; __delay_cycles(300000); }
    if(dir == LEFT  && *col > 0) { buzzer_move(); (*col)--; __delay_cycles(300000); }
    if(dir == UP    && *row > 0) { buzzer_move(); (*row)--; __delay_cycles(300000); }
    if(dir == DOWN  && *row < 2) { buzzer_move(); (*row)++; __delay_cycles(300000); }
}

// =============================================================================
// Menu Navigation
// Moves the menu selection up or down using the joystick
// maxOpt is the highest valid option index
// =============================================================================
void selectOptions(int *opt, int maxOpt)
{
    int dir = readJoystick();
    if(dir == UP   && *opt > 0)      { (*opt)--; __delay_cycles(350000); }
    if(dir == DOWN && *opt < maxOpt) { (*opt)++; __delay_cycles(350000); }
}

// returns 1 if the joystick button was pressed in the menu
// plays a select sound and waits for release so it doesn't repeat
int menuSelected(void)
{
    if(readJoystick() == SELECT)
    {
        buzzer_select();
        while((P3IN & But3) == 0);   // wait for button release
        __delay_cycles(200000);       // debounce delay
        return 1;
    }
    return 0;
}

// waits until the joystick button is released after placing a mark
// prevents the same press from being detected twice
void checkBoosterS2Release(void)
{
    while((P3IN & But3) == 0);  // wait for release
    __delay_cycles(200000);      // debounce delay
}
