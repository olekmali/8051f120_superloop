#include "c8051F120.h"
#include "c8051F120_io.h"

#include "bu_init.h"
#include "bu_watchdog.h"
#include "timer3int.h"

#include <stdint.h>

// Frequencies in Hz
#define INTERRUPT_RATE    1000UL    // Interrupt frequency in Hz
#define LOOP_RATE          100U     // Loop semaphore frequency in Hz (100Hz == 10ms)

// Time delays in milliseconds
#define T_DEBOUNCE           5U     // button debouncing time in number of loops (5 @ 100Hz ~~ 50ms)
#define T_LONG_PRESS       100U     // boundary between short and long depress time in number of loops

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
        if (!SW2) // push button depressed
        {
            // ignore release times shorter than the debounce time
            if ( time_SW2_released>T_DEBOUNCE )
            {
                // check how long it was released and insert any action 
                // that needs to be done if the release time was longer than nnn epochs
            }
            time_SW2_released = 0;

            // check if button is being pressed for longer than T_LONG_PRESS epochs
            if ( time_SW2_pressed>T_LONG_PRESS )
            {
                // code specific to the particular application
            }

            // increment only if still within the range to prevent roll over
            if (time_SW2_pressed<0xffff)
            {
                time_SW2_pressed++;
            }
        }
        else // push button released
        {
            // ignore push times shorter than the debounce time
            if ( time_SW2_pressed>T_DEBOUNCE )
            {
                // check if released after a long period of time
                if (time_SW2_pressed>T_LONG_PRESS)
                {
                    // code specific to the particular application

                    // we arranged for the very last mode to be the off mode
                    // so toggling between a non-ff mode and the off mode looks like this:
                    if (mode==(MAX_MODES-1)) mode=0; else mode=(MAX_MODES-1);
                    Timer3_setMode(mode);
                }
                else // it was pressed for a short period of time but still longer than debounce time
                {
                    // code specific to the particular application

                    // we arranged for the very last mode to be the off mode
                    // so advancement tot he next mode looks like this:
                    mode++; if (mode>=(MAX_MODES-1)) mode=0;
                    Timer3_setMode(mode);
                }
            }
            time_SW2_pressed = 0;               // reset the pressed value so that we handle pressed only once

            if (time_SW2_released<0xffff) {
                time_SW2_released++;                // increment only if still within the range to prevent roll over
            }
        }

    }
}
