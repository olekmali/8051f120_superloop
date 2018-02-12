#include "c8051F120.h"
#include "c8051F120_io.h"

#include "bu_init.h"
#include "bu_watchdog.h"
#include "timer3int.h"

#include <stdint.h>

#define INTERRUPT_RATE  50000UL     // Interrupt frequency in Hz
#define LOOP_RATE         100U      // Loop semaphore frequency in Hz

void main(void)
{
    uint8_t mode    = 0;
    uint16_t  time_SW2_pressed  = 0;
    uint16_t  time_SW2_released = 0;

    // Set up watchdog timer (10.4ms at sysclk of 100MHz)
    WatchDog_set_10ms();

    PORT_Init ();
    SYSCLK_Init();

    Timer3_Init(SYSCLK, INTERRUPT_RATE, LOOP_RATE);
    Timer3_setMode(2);

    EA = 1;                         // enable global interrupts

    while (1)
    {
        // wait for Timer3 interrupt, at the rate of LOOP_RATE
        // it will release the semaphore so that the loop does one iteration
        while( !Timer3_GetSemaphore() ) {
            WatchDog_reset();   // not every interrupt releases the semaphore
                                // reset the watchdog timer while still waiting
            
            // sleep until the next interrupt happens and then check [again]
            // if it is the one we are waiting for to be completed
            CPU_sleep();
        }
        // and immediately afterwards reset the interrupt done marker for the next time
        Timer3_ResetSemaphore();

        // Reset the watchdog timer
        WatchDog_reset();

        
        // code below is executed at the frequency of LOOP_RATE 
        // all timing is expressed by loop counts times (1000ms/LOOP_RATE)
        // in this example the loop rate is 100 and hence the time period is 10ms
        // **********************************************************************

        // handle timing of switches
        if (!SW2)
        {
            if (time_SW2_released>10)               // check if pressed after released for at least 100ms
            {
                time_SW2_released = 0;
            }

            if (time_SW2_pressed<0xffff)
            {
                time_SW2_pressed++;                 // increment only if still within the range to prevent roll over
            }
        }
        else
        {
            if (time_SW2_pressed>10)                // check if was pressed for at least 10 loops - 100ms (debouncing)
            {
                if (time_SW2_pressed>60)            // check if was pressed for more than 60 loops - 600ms
                {
                    // code specific to the particular application
                    if (mode==(MAX_MODES-1)) mode=0;
                    else mode=(MAX_MODES-1);        // we arranged for the very last mode to be the off mode
                    Timer3_setMode(mode);
                }
                else                                // was pressed for not more than 60 loops (but more than debouncing)
                {
                    // code specific to the particular application
                    // advance to the next mode but skip the "off mode"
                    mode++; if (mode>=(MAX_MODES-1)) mode=0;
                    Timer3_setMode(mode);
                }
                time_SW2_pressed = 0;               // reset the pressed value so that we handle pressed only once
            }

            if (time_SW2_released<0xffff) {
                time_SW2_released++;                // increment only if still within the range to prevent roll over
            }
        }

    }
}
