# src/drivers/

Hardware-facing drivers for the MSP430FR6989 breadboard setup.

Put here:
- `joystick.*`: read joystick X/Y + select button (ADC + GPIO) + basic normalization
- `display.*`: low-level pixel display API (set pixel, clear, flush, etc.)
- `buzzer.*`: low-level buzzer output (PWM/timer toggling, tone on/off)

These files should NOT contain Tic-Tac-Toe rules.
They should expose simple functions the rest of the app can call.
