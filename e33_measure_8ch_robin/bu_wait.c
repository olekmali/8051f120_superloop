#include "bu_wait.h"
#include "C8051F120.h"              // Device-specific SFR Definitions

//-----------------------------------------------------------------------------
// wait_ms
//-----------------------------------------------------------------------------
//
// This routine inserts a delay of <ms> milliseconds.
//
void wait_ms(uint32_t clock, uint16_t ms)
{
    uint8_t SFRPAGE_SAVE = SFRPAGE; // Save Current SFR page

    SFRPAGE = TMR2_PAGE;
    TMR2CN = 0x00;                  // Stop Timer2; Clear TF2; 
                                    // Set internal count source
    TMR2CF = 0x00;                  // use SYSCLK/12 as timebase

    RCAP2 = (uint16_t)( 65536U - (clock/(1000U*12U)) );
                                    // Timer 2 set to overflow at 1 kHz
                                    // NOTE: 0 << (clock/(1000*12)) <= 65535
                                    //         or increase timebase speed
    TMR2 = RCAP2;                   // Load it for the first time
                                    // it will be reloaded
                                    // at later times each time it overloads
    ET2 = 0;                        // Disable Timer 2 interrupts
                                    // We don't care if all interrupts are enabled
    TR2 = 1;                        // Start Timer 2

    while(ms>0)
    {
        TF2 = 0;                    // Clear overflow flag
        while(!TF2) ;               // wait until Timer 2 overflows
        ms--;                       // decrease ms to go
    }

    TR2 = 0;                        // Stop Timer 2

    SFRPAGE = SFRPAGE_SAVE;         // Restore SFRPAGE
}
