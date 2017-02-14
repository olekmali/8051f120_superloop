// Copyright (C) 2008-2017 Aleksander Malinowski

#include "C8051F120.h"                         // Device-specific SFR Definitions
#include "C8051F120_io.h"                      // Device-specific SFR Definitions

#include "bu_init.h"

#include <stdint.h>

void main(void)
{
    uint8_t state;

    // Enable the VDD Monitor as a reset source
    // Do not use read-modify write instruction on this register
    // it is needed to access the flash memory
//    RSTSRC = 0x02;

    // Disable watchdog timer
    WDTCN = 0xde;
    WDTCN = 0xad;

    // Initialize the MCU
    PORT_Init();
    SYSCLK_Init();

    state = SW2;
    LED = 1;

    while(1)
    {
        if (state != SW2)
        {   
            if (state==1)
            {
                LED = ! LED;
            } else {
            }
            state = SW2;
        }
    }
}
