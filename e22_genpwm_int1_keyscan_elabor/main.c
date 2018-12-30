#include <C8051F120.h>
#include "C8051F120_io.h"

#include "bu_init.h"
#include "bu_watchdog.h"
#include "timer3int.h"

#include <stdint.h>

#define INTERRUPT_FRQ   5000UL          // Interrupt frequency in Hz
#define PWM_FRQ          250U           // PWM frequency
#define SEMAPHORE_FRQ    100U           // Semaphore frequency

void main(void)
{
    uint8_t rate = 10;
    uint16_t duration_ms = 0;

    // Set up watchdog timer (10.4ms at sysclk of 100MHz)
    WatchDog_set_10ms();

    PORT_Init ();
    SYSCLK_Init();

    Timer3_Init(SYSCLK, INTERRUPT_FRQ, PWM_FRQ, SEMAPHORE_FRQ);
    Timer3_setPWMDuty(rate);
    EA = 1;                             // enable global interrupts

    while(1)
    {
        while( ! semaphore_get() )      // waiting for the next 10ms to start
            ;
        semaphore_reset();
        WatchDog_reset();               // Reset watchdog timer

        if (!SW2)
        {
            if (duration_ms<65535U)
                duration_ms = duration_ms + 10;
            // determining if it is being pressed for a long time
            if (duration_ms>=500) 
            {   
                rate = 0;
                Timer3_setPWMDuty(rate);
            }
        } else {
            // determining if it was a long press or a short press
            if (duration_ms>=500) 
            {   
                duration_ms = 0;
            } else {
                if (duration_ms>0) {
                    rate++;
                    if (rate>100)
                        rate=0;
                    Timer3_setPWMDuty(rate);
                    duration_ms = 0;
                } else {
                    // do nothing, button stays released
                }
            }
        }
    }

}
