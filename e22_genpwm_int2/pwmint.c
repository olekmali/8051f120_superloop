#include "pwmint.h"

#include "C8051F120.h"                  // SFR declarations
#include "C8051F120_io.h"               // SFR declarations


//------------------------------------------------------------------------------------
// Hardware IO CONSTANTS
//------------------------------------------------------------------------------------
__sbit __at (0xB0) PWMout0;             // output bit 0
__sbit __at (0xB1) PWMout1;             // output bit 1
__sbit __at (0xB2) PWMout2;             // output bit 2
__sbit __at (0xB3) PWMout3;             // output bit 3
__sbit __at (0xB4) PWMout4;             // output bit 4
__sbit __at (0xB5) PWMout5;             // output bit 5

//------------------------------------------------------------------------------------
// Global CONSTANTS
//------------------------------------------------------------------------------------
#define PHASE_PREC      (65536L)        // range of phase accumulator
#define FREQUENCY       (1000)          // default frequency for PWM
#define PHASE_ADD_1KHZ (((uint32_t)FREQUENCY)*((uint32_t)PHASE_PREC)/((uint32_t)50000L))    // phase_add for 1kHz PWM

//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------

static uint16_t sampling    = (uint16_t)50000L;   // sampling frequency of output in Hz, defaults to 50 kHz
static uint16_t phase_add   = (uint16_t)PHASE_ADD_1KHZ;

static PWMstate output_offon[NUM_PWM_CHANNELS]          = {OFF, OFF, OFF, OFF, OFF, OFF};    // current state of channel - off/on
static uint16_t dutycount[NUM_PWM_CHANNELS]             = {0,   0,   0,   0,   0,   0};
                                                        // ^^^ duty cycle in timer interrupt ticks, defaults to 0%
static uint8_t  desired_changed = 0;
static PWMstate desired_output_offon[NUM_PWM_CHANNELS]  = {OFF, OFF, OFF, OFF, OFF, OFF};
static uint16_t desired_dutycount[NUM_PWM_CHANNELS]     = {0,   0,   0,   0,   0,   0};

//------------------------------------------------------------------------------------
// Timer4_PWM_Init
//------------------------------------------------------------------------------------
//
// Configure Timer4 to auto-reload and generate an interrupt at interval
// specified by <counts> using SYSCLK/12 as its time base.
//
void Timer4_PWM_Init (uint32_t sysclock, uint32_t rate)
{
    uint8_t SFRPAGE_SAVE = SFRPAGE;     // Save the current SFR page
    uint16_t counts = (uint16_t)( sysclock/(12UL*rate) ); // Note that Timer4 is connected to SYSCLK/12

    sampling = rate;
    phase_add= ((uint32_t)FREQUENCY) * ((uint32_t)PHASE_PREC) / sampling;

    SFRPAGE = CONFIG_PAGE;              // set the SFR page to allow access to the necessary SFRs
    P3MDOUT |= 0x3F;                    // Set P3.0 through P3.5 to push-pull

    SFRPAGE = TMR4_PAGE;                // set the SFR page to allow access to the necessary SFRs
    TMR4CN  = 0x00;                     // Stop Timer4; Clear TF4;
    TMR4CF  = 0x00;                     // use SYSCLK/12 as timebase
//  TMR4CF  = 0x08;                     // use SYSCLK as timebase
    RCAP4   = (uint16_t)(65536UL - counts); // Set the timer reload value to ensure the desired interrupt frequency
    // or   = -counts; -- see the in class comment
    TMR4    = RCAP4;                    // set starting value
    EIE2   |= 0x04;                     // enable Timer4 interrupts - bit 00000100 or ET4 = 1;
    TMR4CN |= 0x04;                     // start Timer4

    SFRPAGE = SFRPAGE_SAVE;             // Restore the original SFR page
}

//------------------------------------------------------------------------------------
// Parameter Control Functions
//------------------------------------------------------------------------------------

void Timer4_PWM_SetFrequency(uint32_t newfrequency)
{
    __bit EA_SAVE     = EA;             // Preserve the current Interrupt Status
    EA = 0;                             // disable interrupts
    if (newfrequency<1) newfrequency=1;
    phase_add = ((uint32_t)newfrequency) * ((uint32_t)PHASE_PREC) / ((uint32_t)sampling);
    EA = EA_SAVE;                       // restore interrupts
}

void Timer4_PWM_SetOn(uint8_t channel, PWMstate newstate)
{
    __bit EA_SAVE     = EA;             // Preserve the current Interrupt Status
    EA = 0;                             // disable interrupts
    desired_output_offon[channel] = newstate;
    desired_changed = 1;
    EA = EA_SAVE;                       // restore interrupts
}

void Timer4_PWM_SetDuty(uint8_t channel, uint8_t newduty) {
    __bit EA_SAVE     = EA;             // Preserve the current Interrupt Status
    EA = 0;                             // disable interrupts
    desired_dutycount[channel] = (uint32_t)PHASE_PREC * newduty / 100;
    desired_changed = 1;
    EA = EA_SAVE;                       // restore interrupts
}

//------------------------------------------------------------------------------------
// Interrupt Service Routines
//------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Timer4_PWM_ISR -- Interrupt-based PWM Generator
//-----------------------------------------------------------------------------
//
// This ISR is called on Timer4 overflows.  Timer4 is set to auto-reload mode.
//
void Timer4_PWM_ISR (void) __interrupt 16 __using 3
{
    static uint16_t phase_current  = 0;   // holds phase accumulator, Note: will roll over at 65536
           uint16_t phase_last;           // used to detect phase roll over through 0

    SFRPAGE = TMR4_PAGE;            // set the SFR page to allow access to the necessary SFRs
    TMR4CN &= ~0x80;                // clear T4 overflow flag, or use sbit TF4=0

    phase_last     = phase_current;
    phase_current += phase_add;     // increment phase accumulator
    // Note: this will roll over PHASE_PREC value per property of uint16_t

    // change PWM only at the beginning of the new cycle to avoid glitches
    if ( phase_last>phase_current && desired_changed) {
        uint8_t i;
        for (i=0; i<NUM_PWM_CHANNELS; ++i) {
            dutycount[i]    = desired_dutycount[i];
            output_offon[i] = desired_output_offon[i];
        }
        desired_changed = 0;
    }


    // You may need to change the SFR page here to one needed to control your particular peripherals
    if ( (output_offon[0] == ON) && (phase_current < dutycount[0]) ) { LED = 1; }     else { LED = 0;} // <- this line is only for visual testing purposes
    if ( (output_offon[0] == ON) && (phase_current < dutycount[0]) ) { PWMout0 = 1; } else { PWMout0 = 0;}
    if ( (output_offon[1] == ON) && (phase_current < dutycount[1]) ) { PWMout1 = 1; } else { PWMout1 = 0; }
    if ( (output_offon[2] == ON) && (phase_current < dutycount[2]) ) { PWMout2 = 1; } else { PWMout2 = 0; }
    if ( (output_offon[3] == ON) && (phase_current < dutycount[3]) ) { PWMout3 = 1; } else { PWMout3 = 0; }
    if ( (output_offon[4] == ON) && (phase_current < dutycount[4]) ) { PWMout4 = 1; } else { PWMout4 = 0; }
    if ( (output_offon[5] == ON) && (phase_current < dutycount[5]) ) { PWMout5 = 1; } else { PWMout5 = 0; }

    // This is an interrupt, the original SFR page will be restored upon return from it
}
