// Copyright (C) 2015-2017 Aleksander Malinowski

#include "bu_init.h"
#include "timer3int.h"
#include "c8051F120.h"                  // Device-specific SFR Definitions
#include "c8051F120_io.h"               // Device-specific SFR Definitions

#include <stdint.h>

#define SAMPLE_RATE     50000           // Interrupt frequency in Hz
#define LOOP_RATE         100           // Loop semaphore frequency in Hz

#define TOGGLE_CNT (LOOP_RATE/2)        // LED flashing to indicate "stay alive"

void main(void)
{
    uint16_t cnt;

    // Set up watchdog timer
    WDTCN = 0xa5;   // reset count
    WDTCN = 0xff;   // disable disabling
    WDTCN = 0x07;   // set time out value to maximum (10.4ms @ 100MHz)

    PORT_Init ();
    SYSCLK_Init();

    Timer3_Init(SYSCLK, SAMPLE_RATE, LOOP_RATE);
 
    // Reduce the main loop rate below 100Hz to see that frequent resets start occuring
    // check it in debugger by using run-to-cursor -- you should reach here only once

    EA = 1;                     // enable global interrupts

    cnt = TOGGLE_CNT;
    while (1)
    {
        // wait for Timer3 interrupt, when it ends
        while( !Timer3_GetSemaphore() ) ;
        // and immediately afterwards reset the interrupt done marker for the next time
        Timer3_ResetSemaphore();

        // Reset watchdog timer
        WDTCN = 0xa5;

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
