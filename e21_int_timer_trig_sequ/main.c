#include <C8051F120.h>
#include "c8051F120_io.h"               // Device-specific SFR Definitions

#include "bu_init.h"
#include "bu_watchdog.h"
#include "timer3int.h"

#include <stdint.h>

#define INTERRUPT_RATE  50000UL         // Interrupt frequency in Hz
#define LOOP_RATE         100U          // Loop semaphore frequency in Hz

#define ACTION_LEN (LOOP_RATE*3U)       // how long to respond to a button push (in seconds)

void main(void)
{
    uint16_t action_cnt;
    uint8_t  count;

    // Set up watchdog timer (10.4ms at sysclk of 100MHz)
    WatchDog_set_10ms();

    PORT_Init ();
    SYSCLK_Init();

    Timer3_Init(SYSCLK, INTERRUPT_RATE, LOOP_RATE);
 
    // Reduce the main loop rate below 100Hz to see that frequent resets start occurring
    // check it in debugger by using run-to-cursor -- you should reach here only once

    EA = 1;                             // enable global interrupts

    action_cnt = 0;
    // initialize action
    LED = 0;
    count = 0;
    while (1)
    {
        // wait for Timer3 interrupt, when it ends
        while( !Timer3_GetSemaphore() )
            ;
        // and immediately afterwards reset the interrupt done marker for the next time
        Timer3_ResetSemaphore();

        WatchDog_reset();               // Reset watchdog timer

        if ( action_cnt>0U )
        {
            action_cnt--;
            if ( 0 == action_cnt ) 
            {
                // shut down action
                LED = 0;
            } else {
                // carry out action
                count--;
                if ( 0 == count )
                {
                    if ( 1==LED )
                    {
                        LED = 0;
                        count = 10;
                    } else {
                        LED = 1;
                        count = 20;
                    }
                }
            }
        } else {
            if ( 0 == SW2 )
            {
                action_cnt = ACTION_LEN;
                // start action
                LED = 1;
                count = 20;
            }
        }
    }
}
