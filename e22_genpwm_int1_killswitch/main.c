#include <C8051F120.h>
#include "C8051F120_io.h"

#include "bu_init.h"
#include "timer3int.h"

#include <stdint.h>

#define INTERRUPT_FRQ   5000UL          // Interrupt frequency in Hz
#define PWM_FRQ          250U           // PWM frequency
#define SEMAPHORE_FRQ    100U           // Semaphore frequency
#define KILLSW_FRQ         1U           // Frequency of PWM update that keeps PWM running

void main(void)
{
    uint8_t state;
    uint8_t rate = 10U;

    // Disable watchdog timer
    WDTCN = 0xde;
    WDTCN = 0xad;

    PORT_Init ();
    SYSCLK_Init();

    Timer3_Init(SYSCLK, INTERRUPT_FRQ, PWM_FRQ, SEMAPHORE_FRQ, KILLSW_FRQ);
    EA = 1;                             // enable global interrupts

    Timer3_setRate(rate);
    state = SW2;
    while(1)
    {
        while( ! semaphore_get() )
            ;
        semaphore_reset();

        // if you don't press a button within 1 second PWM will shut down temporarily
        if (!SW2)
        {
            rate = rate + 10U;
            if (rate>100U)
                rate=0U;
            Timer3_setRate(rate);
        }
    }

}
