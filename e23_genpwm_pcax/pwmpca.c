#include "pwmpca.h"

#include "C8051F120.h"
#include "C8051F120_io.h"


//------------------------------------------------------------------------------------
// Hardware IO CONSTANTS
//------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------
// Global CONSTANTS
//------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// PCA0_Init
//-----------------------------------------------------------------------------
//
// Configure the Programmable Counters
//
void PCA0_Init()
{
    uint8_t SFRPAGE_SAVE = SFRPAGE;     // Save the current SFR page
    uint8_t modeN = 0xCB;               // 0x4- sets counter to 16-bit comparator mode
                                        //      AM: do not set to C to enforce 16bit mode separately
                                        // 0x-8 to enable comparator match to be detected and set
                                        // 0x-1 to enable comparator interrupt after match is detected
                                        // 0x-4 to (toggle) output for PWM - it can be routed to port pins or/and read from reg

    SFRPAGE   = CONFIG_PAGE;            // set the SFR page to allow access to the necessary SFRs
    P0MDOUT  |= 0xFC;                   // Set P0.2 through P0.8 to push-pull, keep current state of P0.0 and P0.1
    XBR0 = 0x30;    // Close UART0, open UART1, _but_ Close CP0 and /INT0 which prevents Ethernet drivers from working

    // ** Don't do it if Ethernet extension board AB_4 is present which is the case in the lab **
    XBR1 = 0x00;    // Open PCA counters 0 through 5 to P0.2 through P0.7 pins which creates problems on Ethernet extensions
    XBR2 = 0x44;    // Enable crossbar and weak pull-up

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
    

//  EIE1     &= ~0x08;                  // Disable PCA0 interrupt at EIE1.3

    SFRPAGE = SFRPAGE_SAVE;             // Restore the original SFR page
}

void PCA0_SetOn(uint8_t channel, PWMstate newstate)
{
    __bit EA_SAVE     = EA;             // Preserve the current Interrupt Status
    EA = 0;                             // disable interrupts
    if (newstate==ON)
    {
        switch(channel)
        {
            case 0: PCA0CPM0 |= 0x02; break;
            case 1: PCA0CPM1 |= 0x02; break;
            case 2: PCA0CPM2 |= 0x02; break;
            case 3: PCA0CPM3 |= 0x02; break;
            case 4: PCA0CPM4 |= 0x02; break;
            case 5: PCA0CPM5 |= 0x02; break;
        }
    } else {
        switch(channel)
        {
            case 0: PCA0CPM0 &= ~0x02; break;
            case 1: PCA0CPM1 &= ~0x02; break;
            case 2: PCA0CPM2 &= ~0x02; break;
            case 3: PCA0CPM3 &= ~0x02; break;
            case 4: PCA0CPM4 &= ~0x02; break;
            case 5: PCA0CPM5 &= ~0x02; break;
        }
    }
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
    // Mr. Dexter Travis Pro tips:
    //since timer counts up from reload to overflow we have to
    //subtract from 100% to get right ratios. Alternately we could
    //negate the bits of the result value after multiply and divide
    //multiply the new % by full scale value (0xFFFF) then
    //divide by 100. To divide by 100 and not lose precision due to
    //integer arithmetic truncation we first divide by 50, check the low
    //order bit and add 1 if that bit is set.  The result is a
    //correctly rounded (if >= .5 round up if less round down)

    lo = (uint8_t) value;
    hi = (uint8_t) (value >> 8);

    // Note: PCA0 requires that we load low byte first and must follow with high byte next
    SFRPAGE   = PCA0_PAGE;              // set the SFR page to access PCA
    switch(channel) {
        case 0: PCA0CPL0 = lo; PCA0CPH0 = hi; break;
        case 1: PCA0CPL1 = lo; PCA0CPH1 = hi; break;
        case 2: PCA0CPL2 = lo; PCA0CPH2 = hi; break;
        case 3: PCA0CPL3 = lo; PCA0CPH3 = hi; break;
        case 4: PCA0CPL4 = lo; PCA0CPH4 = hi; break;
        case 5: PCA0CPL5 = lo; PCA0CPH5 = hi; break;
    }

    // Mr. Dexter Travis Pro tips:
    // Do not update immediately as we do it here, use a buffer to store the new value and
    // read from it on the PCA0 counter overflow to avoid strange spikes

    EA = EA_SAVE;                       // restore interrupts
    SFRPAGE = SFRPAGE_SAVE;             // Restore the original SFR page
}
