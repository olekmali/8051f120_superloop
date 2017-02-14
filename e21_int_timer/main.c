// Copyright (C) 2008-2017 Aleksander Malinowski

#include "C8051F120.h"                  // Device-specific SFR Definitions
#include "C8051F120_io.h"               // SFR declarations

#include "bu_init.h"
#include "timer3int.h"

#include <stdint.h>

#define SAMPLE_RATE 50000L              // Interrupt frequency in Hz
#define PULSE_FRQ   25000
#define PULSE_WDT    5000

// extern uint8_t Timer3_rate1, Timer3_rate2; 
// cannot access Timer3_rateN from here because of used "static" keyword
// in timer3int.c that makes the global variable name visible only in one module

void main(void)
{
    uint8_t state;
    uint8_t mode;

    // Disable watchdog timer
    WDTCN = 0xde;
    WDTCN = 0xad;

    PORT_Init ();
    SYSCLK_Init();

    Timer3_Init(SYSCLK, SAMPLE_RATE);   // Init Timer3 to generate interrupts at a SAMPLE_RATE rate
    EA = 1;                             // enable global interrupts

    mode  = 1;
    Timer3_setRates(PULSE_WDT, PULSE_FRQ );
    Timer3_setOffOn(1);

    state = SW2;
    while(1)
    {
        if (state != SW2)
        {   
            if (state)
            {
                mode++; if (mode>5) mode=0;     // mode = (mode+1) % 6;

                Timer3_setRates( ((uint32_t)(mode)*PULSE_FRQ/5), PULSE_FRQ );
                                // Q: why typecasting to int32_t here?
                
            }
            state = SW2;  
        }
        // ** if the switch is not debounced then insert some delay here ***
    }

}
