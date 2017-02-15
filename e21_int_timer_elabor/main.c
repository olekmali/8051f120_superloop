// Copyright (C) 2013-2017 Aleksander Malinowski

#include "c8051F120.h"                  // Device-specific SFR Definitions
#include "c8051F120_io.h"               // Device-specific SFR Definitions

#include "bu_init.h"
#include "bu_watchdog.h"
#include "timer3int.h"

#include <stdint.h>

#define SAMPLE_RATE     50000           // Interrupt frequency in Hz
#define LOOP_RATE         100           // Loop semaphore frequency in Hz

void main(void)
{
    uint8_t mode    = 0;
    uint16_t  time_SW2_pressed  = 0;
    uint16_t  time_SW2_released = 0;

    // Set up watchdog timer (10.4ms at sysclk of 100MHz)
    WatchDog_set_10ms();

    PORT_Init ();
    SYSCLK_Init();

    Timer3_Init(SYSCLK, SAMPLE_RATE, LOOP_RATE);
    Timer3_setMode(2);

    EA = 1;                     // enable global interrupts

    while (1)
    {
        // wait for Timer3 interrupt, when it ends
        while( !Timer3_GetSemaphore() ) {
            // WatchDog_reset();    // optionally reset watchdog timer while waiting
                                    // for the right interrupt to be completed
            
            // sleep until an interrupt happens and then check [again]
            // if it is the one we are waiting for to be completed
            CPU_sleep();
        }
        // and immediately afterwards reset the interrupt done marker for the next time
        Timer3_ResetSemaphore();

        // Reset watchdog timer
        WatchDog_reset();

        // handle timing of switches
        if (!SW2)
        {
            if (time_SW2_released>10)             // check if pressed after released for at least 100ms
            {
                time_SW2_released = 0;
            }

            if (time_SW2_pressed<0xffff) time_SW2_pressed++;  // increment only if still within range
        }
        else
        {
            if (time_SW2_pressed>10)                // check if released after pressed for at least 100ms
            {
                if (time_SW2_pressed>60)            // int32_t press, int32_ter than 600ms
                {
                    if (mode==(MAX_MODES-1)) mode=0;
                    else mode=(MAX_MODES-1);        // we arranged for the very last mode to be off mode
                    Timer3_setMode(mode);
                }
                else                                // short press (but int32_ter than 100ms)
                {
                    // advance to the next mode but skip the "off mode"
                    mode++; if (mode>=(MAX_MODES-1)) mode=0;
                    Timer3_setMode(mode);
                }
                time_SW2_pressed = 0;               // reset the pressed value so that we handle pressed only once
            }

            if (time_SW2_released<0xffff) time_SW2_released++;  // increment only if still within range
        }

    }
}
