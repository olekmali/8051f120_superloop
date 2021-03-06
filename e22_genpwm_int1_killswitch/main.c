#include <C8051F120.h>
#include "C8051F120_io.h"

#include "bu_init.h"
#include "bu_watchdog.h"
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

    // Set up watchdog timer (10.4ms at sysclk of 100MHz)
    WatchDog_set_10ms();

    PORT_Init ();
    SYSCLK_Init();

    Timer3_Init(SYSCLK, INTERRUPT_FRQ, PWM_FRQ, SEMAPHORE_FRQ, KILLSW_FRQ);
    Timer3_setPWMDuty(rate);
    EA = 1;                             // enable global interrupts

    state = SW2;
    while(1)
    {
        while( ! semaphore_get() )
            ;
        semaphore_reset();
        WatchDog_reset();               // Reset watchdog timer

        // if you don't press a button within 1 second PWM will shut down temporarily
        if (state!=SW2)
        {
            state=SW2;
            if (!SW2)
            {
                rate = rate + 5U;
                if (rate>100U)
                    rate=0U;
                Timer3_setPWMDuty(rate);
            }
        }
    }

}
