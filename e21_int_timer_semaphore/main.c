// Copyright (C) 2015-2017 Aleksander Malinowski

#include "bu_init.h"
#include "timer3int.h"
#include "c8051F120.h"                  // Device-specific SFR Definitions
#include "c8051F120_io.h"               // Device-specific SFR Definitions

#define SAMPLE_RATE     50000           // Interrupt frequency in Hz
#define LOOP_RATE           4           // Loop semaphore frequency in Hz

void main(void)
{
    // Disable watchdog timer
    WDTCN = 0xde;
    WDTCN = 0xad;

    PORT_Init ();
    SYSCLK_Init();

    Timer3_Init(SYSCLK, SAMPLE_RATE, LOOP_RATE);
 
    EA = 1;                     // enable global interrupts

    while (1)
    {
        // wait for Timer3 interrupt, when it ends
        while( !Timer3_GetSemaphore() ) ;
        // and immediately afterwards reset the interrupt done marker for the next time
        Timer3_ResetSemaphore();

        LED = !LED;
    }
}
