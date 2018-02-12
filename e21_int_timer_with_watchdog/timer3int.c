#include "timer3int.h"

#include "c8051f120.h"
#include "c8051f120_io.h"


//------------------------------------------------------------------------------------
// Global variable(s) used as bridge to pass parameters to the interrupts
//------------------------------------------------------------------------------------
static uint8_t Timer3_semaphore   = 0;
static uint16_t  Timer3_sem_frequ   = 0;

//------------------------------------------------------------------------------------
// Timer3_Init
//------------------------------------------------------------------------------------
//
// Configure Timer3 to auto-reload and generate an interrupt at interval
// specified by <counts>
//
void Timer3_Init (uint32_t sysclock, uint32_t rate, uint16_t timing)
{
    uint8_t SFRPAGE_SAVE = SFRPAGE;     // Save the current SFR page
    uint16_t counts = (uint16_t)( sysclock/(12UL*rate) ); // Init Timer3 to generate interrupts at a RATE Hz rate.
                                        // Note that timer3 is connected to SYSCLK/12

    Timer3_sem_frequ = rate/timing;     // Set the semaphore frequency

    SFRPAGE = TMR3_PAGE;                // set the SFR page to allow access to the necessary SFRs
    TMR3CN  = 0x00;                     // Stop Timer3; Clear TF3;
    TMR3CF  = 0x00;                     // use SYSCLK/12 as timebase
//  TMR3CF  = 0x08;                     // use SYSCLK as timebase
    RCAP3   = (uint16_t)(65536UL - counts); // Set the timer reload value to ensure the desired interrupt frequency
    // or   = -counts; -- see the in class comment
    TMR3    = 0xffff;                   // set to reload immediately
    EIE2   |= 0x01;                     // enable Timer3 interrupts
    TMR3CN |= 0x04;                     // start Timer3

    SFRPAGE = SFRPAGE_SAVE;             // Restore the original SFR page
}


//------------------------------------------------------------------------------------
// Functions used to access the global variables shared with interrupts
//------------------------------------------------------------------------------------
void Timer3_ResetSemaphore()
{
    Timer3_semaphore = 0;               // Atomic operation - no need to disable interrupts
}

char Timer3_GetSemaphore()
{
    return(Timer3_semaphore);           // Atomic operation - no need to disable interrupts
}


//------------------------------------------------------------------------------------
// Interrupt Service Routines
//------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------
// Timer3_ISR
//------------------------------------------------------------------------------------
// This routine changes the state of the LED whenever Timer3 overflows.
//
// NOTE: The SFRPAGE register will automatically be switched to the Timer 3 Page
// When an interrupt occurs.  SFRPAGE will return to its previous setting on exit
// from this routine.
//
void Timer3_ISR (void) __interrupt 14
{
    // static variable - a global variable hidden in a function
    static uint16_t  sem_cnt = 0;
    
    SFRPAGE  = TMR3_PAGE;
    TF3 = 0;                        // clear TF3 so that the interrupt may happen again

    if (sem_cnt==0)
    {
        sem_cnt = Timer3_sem_frequ;
        Timer3_semaphore  = 1;
    } else {
        --sem_cnt;
    }

    // *** insert your custom interrupt code here ***
}
