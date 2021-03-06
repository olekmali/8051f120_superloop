#include <C8051F120.h>
#include "C8051F120_io.h"

#include "bu_init.h"
#include "bu_watchdog.h"
#include "timer3int.h"

#include <stdint.h>

#define INTERRUPT_RATE  50000UL         // Interrupt frequency in Hz
#define PULSE_FRQ       25000U
#define PULSE_WDT        5000U

// extern uint8_t Timer3_rate1, Timer3_rate2; 
// cannot access Timer3_rateN from here because of used "static" keyword
// in timer3int.c that makes the global variable name visible only in one module

void main(void)
{
    uint8_t state;
    uint8_t mode;

    WatchDog_disable_all();             // Disable watchdog timer

    PORT_Init ();
    SYSCLK_Init();

    Timer3_Init(SYSCLK, INTERRUPT_RATE);// Init Timer3 to generate interrupts at a INTERRUPT_RATE rate
    Timer3_setRates(PULSE_WDT, PULSE_FRQ );
    Timer3_setOffOn(1);
    EA = 1;                             // enable global interrupts

    mode  = 1;

    state = SW2;
    while(1)
    {
        if (state != SW2)
        {   
            if (state)
            {
                mode++; if (mode>5) mode=0;     // mode = (mode+1) % 6;

                Timer3_setRates( ((uint32_t)(mode)*PULSE_FRQ/5U), PULSE_FRQ );
                                // Q: why typecasting to int32_t here?
                
            }
            state = SW2;  
        }
        // ** if the switch is not debounced then insert some delay here ***
    }

}
