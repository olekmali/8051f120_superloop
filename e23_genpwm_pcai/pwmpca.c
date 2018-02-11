#include "pwmpca.h"

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

//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------
uint8_t PCA0CN_mask = 0xBF;       // mask for bits: 10111111, see doc for PCA0CN

//-----------------------------------------------------------------------------
// PCA0_Init
//-----------------------------------------------------------------------------
//
// Configure the Programmable Counters
//
void PCA0_Init()
{
    uint8_t SFRPAGE_SAVE = SFRPAGE;     // Save the current SFR page
    uint8_t modeN = 0x49;               // 0x4- sets counter to 16-bit comparator mode
                                        //      AM: do not set to C to enforce 16bit mode separately
                                        // 0x-8 to enable comparator match to be detected and set
                                        // 0x-1 to enable comparator interrupt after match is detected
                                        // 0x-2 to (toggle) output for PWM - it can be routed to port pins or/and read from reg

    SFRPAGE   = CONFIG_PAGE;            // set the SFR page to allow access to the necessary SFRs
    P3MDOUT  |= 0x3F;                   // Set P3.0 through P3.5 to push-pull

    SFRPAGE   = PCA0_PAGE;              // set the SFR page to allow access to the necessary SFRs
    PCA0CN    = 0x00;                   // reset counter interrupt bits and disable the counter -0------
    PCA0MD    = 0x03;                   // ----SRC- controls the SouRCe of the counting
                                        //     000  - SYSCLK/12
                                        //     001  - SYSCLK/4
                                        //     010  - Timer0 overflow (but Timer0 will be used later for something)
                                        //     100  - SYSCK, also can use external inputs and external oscillator/8
                                        // -------1 - enables counter roll-over interrupt

    // Program all of the comparators the same way
    PCA0CPM0  = modeN;
    PCA0CPM1  = modeN;
    PCA0CPM2  = modeN;
    PCA0CPM3  = modeN;
    PCA0CPM4  = modeN;
    PCA0CPM5  = modeN;

    // Important: set the low byte first (stops comparing), and the high byte next (starts comparing again)
    PCA0CPL0  = 0x00;
    PCA0CPH0  = 0x20;
    PCA0CPL1  = 0x00;
    PCA0CPH1  = 0x40;
    PCA0CPL2  = 0x00;
    PCA0CPH2  = 0x60;
    PCA0CPL3  = 0x00;                   // Duty cycle of 50% for CP3
    PCA0CPH3  = 0x80;                   // Duty cycle of 50% for CP3
    PCA0CPL4  = 0x00;
    PCA0CPH4  = 0xA0;
    PCA0CPL5  = 0x00;
    PCA0CPH5  = 0xC0;


    PCA0L     = 0x00;                   // Reset PCA Counter Value to 0x0000
    PCA0H     = 0x00;

    PCA0CN   |= 0x40;                   // enable the counter - -1------

    SFRPAGE   = CONFIG_PAGE;
    EIE1     |= 0x08;                   // Enable PCA0 interrupt at EIE1.3

    SFRPAGE = SFRPAGE_SAVE;             // Restore the original SFR page
}

//-----------------------------------------------------------------------------
// PCA0_ISR -- PWM Wave Generator
//-----------------------------------------------------------------------------
//
// This ISR is called on event in any of PCA0 comparators or PCA0 counter overflow.
// If we could output PCA0CN directly to P0.3-P0.6 then manual copying to P3
// would have been unnecessary. However, crossbar is set tu map /INT0 to P0.3
// that is used by Ethernet board and TCP/IP stack library later on
//
void PCA0_ISR (void) __interrupt 9  __using 3
{
    uint8_t intsrc;
    SFRPAGE = PCA0_PAGE;                // set the SFR page to allow access to the necessary SFRs
    intsrc  = PCA0CN & PCA0CN_mask;     // check the interrupt source and mask it with action enable register
    PCA0CN  = 0x40;                     // We always must clear PCA0 interrupt flags manually!

    //P3  = (P3 & 0xC0) | ( PCA0CN & PCA0CN_mask /* & 0x3F */); // preserve two highest bits, and copy the six lowest bits
    if (intsrc & 0x01) P3 |= 0x01;
    if (intsrc & 0x02) P3 |= 0x02;
    if (intsrc & 0x04) P3 |= 0x04;
    if (intsrc & 0x08) P3 |= 0x08;
    if (intsrc & 0x10) P3 |= 0x10;
    if (intsrc & 0x20) P3 |= 0x20;
    if (intsrc & 0x80) P3 &= 0xC0;      // reset all outputs to 0 on counter overflow

    // This is an interrupt, the original SFR page will be restored upon return from it
}

void PCA0_SetOn(uint8_t channel, PWMstate newstate)
{
    __bit EA_SAVE     = EA;             // Preserve the current Interrupt Status
    EA = 0;                             // disable interrupts
    if (newstate==ON)
    {
        PCA0CN_mask |=  (0x01<<channel);
    } else {
        PCA0CN_mask &= (~(0x01<<channel))&0xBF;
    }
    PCA0CN_mask |= 0x80;                // just in case channel>6 to recover overflow detection
    EA = EA_SAVE;                       // restore interrupts
}

void PCA0_SetDuty(uint8_t channel, uint8_t newduty)
{
    uint16_t value;
    uint8_t hi, lo;
    uint8_t SFRPAGE_SAVE = SFRPAGE;     // Save the current SFR page
    __bit EA_SAVE     = EA;             // Preserve the current Interrupt Status
    EA = 0;                             // disable interrupts

    value = (uint16_t)((((( 100 - newduty) * 0xFFFFL) / 50) +1)>>1);
    // Mr. Dexter Travis Pro tips (sic):
    // "since timer counts up from reload to overflow we have to
    // subtract from 100% to get right ratios. Alternately we could
    // negate the bits of the result value after multiply and divide
    // multiply the new % by full scale value (0xFFFF) then
    // divide by 100. To divide by 100 and not lose precision due to
    // integer arithmetic truncation we first divide by 50, check the low
    // order bit and add 1 if that bit is set.  The result is a
    // correctly rounded (if >= .5 round up if less round down)"

    lo = (uint8_t) value;
    hi = (uint8_t) (value >> 8);

    // Note: PCA0 requires that we load low byte first and must follow with high byte next
    SFRPAGE   = PCA0_PAGE;              // set the SFR page to allow access to the necessary SFRs
    switch(channel) {
        case 0: PCA0CPL0 = lo; PCA0CPH0 = hi; break;
        case 1: PCA0CPL1 = lo; PCA0CPH1 = hi; break;
        case 2: PCA0CPL2 = lo; PCA0CPH2 = hi; break;
        case 3: PCA0CPL3 = lo; PCA0CPH3 = hi; break;
        case 4: PCA0CPL4 = lo; PCA0CPH4 = hi; break;
        case 5: PCA0CPL5 = lo; PCA0CPH5 = hi; break;
    }

    // Mr. Dexter Travis Pro tips:
    // "Do not update immediately as we do it here, 
    // use a buffer to store the new value and read from it
    // on the PCA0 counter overflow to avoid strange spikes"

    EA = EA_SAVE;                       // restore interrupts
    SFRPAGE = SFRPAGE_SAVE;             // Restore the original SFR page
}
