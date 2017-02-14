// Copyright (C) 2008-2017 Aleksander Malinowski

#include "C8051F120.h"                  // Device-specific SFR Definitions
#include "C8051F120_io.h"               // SFR declarations

#include "bu_init.h"
#include "timer3int.h"

#include "test_cfg.h"                   // control which experiment to run

#include <stdint.h>

#define SAMPLE_RATE 50000L              // Interrupt frequency in Hz
#define ACTION_RATE 25000


#ifndef use_function_to_change_parameter
    #ifndef force_volatile_parameter_variable
        extern uint8_t Timer3_parameter;
    #else
        extern volatile uint8_t Timer3_parameter;
    #endif
#endif
    
void main(void)
{
    uint8_t state;

    // Disable watchdog timer
    WDTCN = 0xde;
    WDTCN = 0xad;

    PORT_Init ();
    SYSCLK_Init();

    Timer3_Init(SYSCLK, SAMPLE_RATE, ACTION_RATE);
                                        // Init Timer3 to generate interrupts at a SAMPLE_RATE rate
    EA = 1;                             // enable global interrupts

    state = SW2;
    LED = 1;
#ifdef use_function_to_change_parameter
    Timer3_setParam(1);
#else
    Timer3_parameter = 1;
#endif
    
    while(1)
    {
        if (state != SW2)
        {   
            if (state /* !=0 */)
            {
                // switch depresses

#ifdef use_function_to_change_parameter
                if (Timer3_getParam() /* ==1 */ )
                {
                        Timer3_setParam(0);
                } else {
                        Timer3_setParam(1);
                }
#else
                EA = 0;
                if (Timer3_parameter /* ==1 */ )
                {
                        Timer3_parameter = 0;
                } else {
                        Timer3_parameter = 1;
                }
                EA = 1;
#endif
            // } else {
            //  // switch released
            }
            state = SW2;  
        }
    }

}
