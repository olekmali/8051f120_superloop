#include "timer3int.h"

#include <C8051F120.h>
#include "C8051F120_io.h"

//------------------------------------------------------------------------------------
// Global variable(s) used as bridge to pass parameters to the interrupts
//------------------------------------------------------------------------------------
static uint8_t semaphore     = 0;
static uint8_t pwm_rate      = 0;

static uint16_t semaph_epoch = 1;
static uint16_t pwm_epoch    = 100;

//------------------------------------------------------------------------------------
// Timer3_Init
//------------------------------------------------------------------------------------
//
// Configure Timer3 to auto-reload and generate an interrupt at interval
// specified by <counts>
void Timer3_Init (uint32_t sysclock, uint32_t rate, uint32_t rate_pwm, uint32_t rate_semaph)
{
    uint8_t SFRPAGE_SAVE = SFRPAGE;     // Save the current SFR page
    uint16_t counts = (uint16_t)( sysclock/(12UL*rate) ); // Init Timer3 to generate interrupts at a RATE Hz rate.
                                        // Note that timer3 is connected to SYSCLK/12
    pwm_epoch    = rate / rate_pwm;     // how many interrupts per PWM period (controls PWM frequency)
    semaph_epoch = rate / rate_semaph;  // how many interrupts between "giving" semaphore
                                        
    SFRPAGE = TMR3_PAGE;                // set the SFR page to allow access to the necessary SFRs
    TMR3CN  = 0x00;                     // Stop Timer3; Clear TF3;
    TMR3CF  = 0x00;                     // use SYSCLK/12 as timebase
//  TMR3CF  = 0x08;                     // use SYSCLK as timebase
    RCAP3   = (uint16_t)(65536UL - counts); // Set the timer reload value to ensure the desired interrupt frequency
    // or   = -counts; -- see the in class comment
    TMR3    = 0xffff;                   // set to reload immediately
    EIE2   |= 0x01;                     // enable Timer3 interrupts - bit 00000001 or ET3 = 1;

    TMR3CN |= 0x04;                     // start Timer3
    SFRPAGE = SFRPAGE_SAVE;             // Restore the original SFR page
}


//------------------------------------------------------------------------------------
// Functions used to access the global variables shared with interrupts
//------------------------------------------------------------------------------------
void Timer3_setPWMDuty(uint8_t new_rate)
{
    uint8_t EA_SAVE;
    new_rate = (uint8_t)((uint16_t)new_rate * pwm_epoch / 100U );
    EA_SAVE = EA;                       // Preserve Current Interrupt Status
    EA = 0;                             // disable interrupts
    pwm_rate = new_rate;                // percentage of PWM period the output is on/high
    EA = EA_SAVE;                       // restore interrupts
}

void semaphore_reset()
{
    semaphore = 0;
}

uint8_t semaphore_get()
{
    return(semaphore);
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
    static uint8_t  pwm_counter  = 0;
    static uint8_t  cur_pwm_rate = 0;
    static uint16_t semaphore_counter = 0;

    // we are on TMR3_PAGE page right now
    // SFRPAGE = TMR3_PAGE;             // set the SFR page to allow access to the necessary SFRs
    TF3 = 0;                            // clear TF3

    pwm_counter++;
    if (pwm_counter>=pwm_epoch)
    {
        pwm_counter = 0;
        cur_pwm_rate = pwm_rate;
    }

    if (pwm_counter>=cur_pwm_rate)
    {
        LED = 0;
    } else {
        LED = 1;
    }

    semaphore_counter++;
    if (semaphore_counter>=semaph_epoch)
    {
        semaphore_counter = 0;
        semaphore = 1;
    }
}
