#include "timer3int.h"

#include "C8051F120.h"                  // SFR declarations
#include "C8051F120_io.h"               // SFR declarations

//------------------------------------------------------------------------------------
// Global variable(s) used as bridge to pass parameters to the interrupts
//------------------------------------------------------------------------------------
#ifndef force_volatile_parameter_variable
    uint16_t  Timer3_parameter = 1;
#else
    volatile uint16_t  Timer3_parameter = 1;
#endif

static uint16_t  Timer3_rate2 = 0;

//------------------------------------------------------------------------------------
// Timer3_Init
//------------------------------------------------------------------------------------
//
// Configure Timer3 to auto-reload and generate an interrupt at interval
// specified by <counts>
void Timer3_Init (uint32_t sysclock, uint32_t rate, uint16_t rate2)
{
    uint8_t SFRPAGE_SAVE = SFRPAGE;     // Save the current SFR page
    uint16_t counts = (uint16_t)( sysclock/(12UL*rate) ); // Init Timer3 to generate interrupts at a RATE Hz rate.
                                        // Note that timer3 is connected to SYSCLK/12

    SFRPAGE = TMR3_PAGE;                // set the SFR page to allow access to the necessary SFRs
    TMR3CN  = 0x00;                     // Stop Timer3; Clear TF3;
    TMR3CF  = 0x00;                     // use SYSCLK/12 as timebase
//  TMR3CF  = 0x08;                     // use SYSCLK as timebase
    RCAP3   = (uint16_t)(65536UL - counts); // Set the timer reload value to ensure the desired interrupt frequency
    // or   = -counts; -- see the in class comment
    TMR3    = 0xffff;                   // set to reload immediately
    EIE2   |= 0x01;                     // enable Timer3 interrupts - bit 00000001 or ET3 = 1;

    Timer3_rate2 = rate2;               // Timer was set off, reprogrammed and is ready
                                        // but its interrupt is still off
                                        // so it is the best time to update interrupt parameters

    TMR3CN |= 0x04;                     // start Timer3
    SFRPAGE = SFRPAGE_SAVE;             // Restore the original SFR page
}


//------------------------------------------------------------------------------------
// Functions used to access the global variables shared with interrupts
//------------------------------------------------------------------------------------
#ifdef use_function_to_change_parameter

    void Timer3_setParam(uint8_t onoff)
    {
#ifdef do_not_use_bit_variable
        uint8_t EA_SAVE = EA;
#else
        __bit EA_SAVE = EA;
#endif
        EA = 0;
        Timer3_parameter = onoff;
        EA = EA_SAVE;
    }


    uint8_t Timer3_getParam(void)
    {
#ifdef do_not_use_bit_variable
        uint8_t EA_SAVE = EA;
#else
        __bit EA_SAVE = EA;
#endif
        uint8_t value;
        EA = 0;
        value = Timer3_parameter;
        EA = EA_SAVE;
        return(value);
    }

#endif


//------------------------------------------------------------------------------------
// Interrupt Service Routine for Timer3
//------------------------------------------------------------------------------------
//
// This routine changes the state of the LED whenever Timer3 overflows.
//
// NOTE: The SFRPAGE register will automatically be switched to the Timer 3 Page
// When an interrupt occurs.  SFRPAGE will return to its previous setting on exit
// from this routine.
//
#ifndef static_local_interrupt_variable
    static uint16_t counter = 0;
#endif

void Timer3_ISR (void) __interrupt 14
{
#ifdef static_local_interrupt_variable
    static uint16_t counter = 0;
#endif

    // we are on TMR3_PAGE page right now
    // SFRPAGE = TMR3_PAGE;             // set the SFR page to allow access to the necessary SFRs
    TF3 = 0;                            // clear TF3
    counter++;

    if (counter>=Timer3_rate2)
    {
        counter = 0;
        if (Timer3_parameter)
            LED = ! LED;
    }

}
