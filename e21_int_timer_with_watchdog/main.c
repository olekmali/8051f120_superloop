#include <C8051F120.h>
#include "c8051F120_io.h"               // Device-specific SFR Definitions

#include "bu_init.h"
#include "bu_watchdog.h"
#include "timer3int.h"

#include <stdint.h>

#define INTERRUPT_RATE  50000UL         // Interrupt frequency in Hz
#define LOOP_RATE         100U          // Loop semaphore frequency in Hz

#define TOGGLE_CNT (LOOP_RATE/2)        // LED flashing to indicate "stay alive"

void main(void)
{
    uint16_t cnt;

    // Set up watchdog timer (10.4ms at sysclk of 100MHz)
    WatchDog_set_10ms();

    PORT_Init ();
    SYSCLK_Init();

    Timer3_Init(SYSCLK, INTERRUPT_RATE, LOOP_RATE);
 
    // Reduce the main loop rate below 100Hz to see that frequent resets start occurring
    // check it in debugger by using run-to-cursor -- you should reach here only once

    EA = 1;                             // enable global interrupts

    cnt = TOGGLE_CNT;
    while (1)
    {
        // wait for Timer3 interrupt, when it ends
        while( !Timer3_GetSemaphore() )
            ;
        // and immediately afterwards reset the interrupt done marker for the next time
        Timer3_ResetSemaphore();

        WatchDog_reset();               // Reset watchdog timer

        // demo: keep alive indicator
        // experiment: decrease LOOP_RATE to see that
        // this code is unreachable due to RESETs
        cnt--; if (0==cnt) 
        {
            cnt = TOGGLE_CNT;
            LED = !LED;
        }
    }
}
