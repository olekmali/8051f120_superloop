#include <C8051F120.h>
#include "C8051F120_io.h"

#include "bu_init.h"
#include "timer3int.h"

#include <stdint.h>

#define INTERRUPT_FRQ   5000UL          // Interrupt frequency in Hz
#define PWM_FRQ          250U           // PWM frequency

// extern uint8_t Timer3_rate;
// cannot access Timer3_rate from here because of used "static" keyword
// in timer3int.c that makes the global variable name visible only in one module

void main(void)
{
    uint8_t state;
    uint8_t rate = 10U;

    // Disable watchdog timer
    WDTCN = 0xde;
    WDTCN = 0xad;

    PORT_Init ();
    SYSCLK_Init();

    Timer3_Init(SYSCLK, INTERRUPT_FRQ, PWM_FRQ);
    EA = 1;                             // enable global interrupts

    Timer3_setRate(rate);
    state = SW2;
    while(1)
    {
        if (state!=SW2)
        {
            state=SW2;
            if (!SW2)
            {
                rate = rate + 5U;
                if (rate>100U)
                    rate=0U;
                Timer3_setRate(rate);
            }
        }
        // Add some delay if button is not debounced
    }
}
