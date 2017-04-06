#include "timer3int.h"

#include "C8051F120.h"                  // SFR declarations
#include "C8051F120_io.h"               // SFR declarations

//------------------------------------------------------------------------------------
// Global variable(s) used as bridge to pass parameters to the interrupts
//------------------------------------------------------------------------------------
static /* volatile */ uint8_t   Timer3_offon = 1;
static /* volatile */ uint16_t  Timer3_rate1 = 0;
static /* volatile */ uint16_t  Timer3_rate2 = 0;


//------------------------------------------------------------------------------------
// Timer3_Init
//------------------------------------------------------------------------------------
//
// Configure Timer3 to auto-reload and generate an interrupt at interval
// specified by <counts>
void Timer3_Init (uint32_t sysclock, uint32_t rate)
{
    uint8_t SFRPAGE_SAVE = SFRPAGE;     // Save Current SFR page
    uint16_t counts = (uint16_t)( sysclock/(12L*rate) ); // Init Timer3 to generate interrupts at a RATE Hz rate.
                                        // Note that timer3 is connected to SYSCLK/12

    SFRPAGE = TMR3_PAGE;                // set SFR page
    TMR3CN  = 0x00;                     // Stop Timer3; Clear TF3;
    TMR3CF  = 0x00;                     // use SYSCLK/12 as timebase
//  TMR3CF  = 0x08;                     // use SYSCLK as timebase
    RCAP3   = (uint16_t)( 65536U - counts ); // Init reload values
    // or   = -counts; -- see the in class comment
    TMR3    = 0xffff;                   // set to reload immediately
    EIE2   |= 0x01;                     // enable Timer3 interrupts - bit 00000001 or ET3 = 1;

    TMR3CN |= 0x04;                     // start Timer3
    SFRPAGE = SFRPAGE_SAVE;             // Restore SFR page
}


//------------------------------------------------------------------------------------
// Functions used to access the global variables shared with interrupts
//------------------------------------------------------------------------------------
void Timer3_setOffOn(uint8_t onoff)
{
    __bit EA_SAVE       = EA;           // Preserve Current Interrupt Status
    EA = 0;                             // disable interrupts
    
    Timer3_offon = onoff;           // disabling and enabling
                                        // interrupts is actually needed only 
                                        // if you have more than one byte to 
                                        // communicate
    
    EA = EA_SAVE;                       // restore interrupts
}


uint8_t Timer3_getOffOn(void)
{
    __bit EA_SAVE       = EA;           // Preserve Current Interrupt Status
    uint8_t value;
    EA = 0;                             // disable interrupts
    
    value = Timer3_offon;
    // disabling and enabling interrupts is actually needed only 
    // if you have more than one byte to change at a time 
    // and you cannot afford having the interrupt service routine
    // run one some old and some new data at the time of transition
    
    EA = EA_SAVE;                       // restore interrupts
    return(value);
}

void Timer3_setRates(uint16_t rate1, uint16_t rate2)
{
    __bit EA_SAVE       = EA;           // Preserve Current Interrupt Status
    EA = 0;                             // disable interrupts

    Timer3_rate1 = rate1;               
    Timer3_rate2 = rate2;

    EA = EA_SAVE;                       // restore interrupts
}



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
void Timer3_ISR (void) __interrupt 14
{
    static uint16_t counter = 0; // static variable - a global variable 
    // with only a function scope - i.e. hidden inside a function
    // Note: SDCC has an error, actually it is not initialized if declared as global 
    //       variable outside of main.c

    // we are on TMR3_PAGE page right now
    // SFRPAGE = TMR3_PAGE;             // set SFR page
    TF3 = 0;                            // clear TF3

    counter++;
    if (counter>=Timer3_rate2)
    {
        counter = 0;
        LED = 0;
    }

    if (counter==Timer3_rate1)
    {
        if (Timer3_offon)
        LED = 1;
    }
}
