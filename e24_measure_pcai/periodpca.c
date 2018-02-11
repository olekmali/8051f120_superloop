#include "periodpca.h"
#include "C8051F120.h"                  // SFR declarations

//------------------------------------------------------------------------------------
// Hardware IO CONSTANTS
//------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------
// Global CONSTANTS
//------------------------------------------------------------------------------------
// Note: 2^MAX_HISTDIV == MAX_HISTORY  so that  (sum >> MAX_HISTDIV) denotes average
#define MAX_HISTORY (4)
#define MAX_HISTDIV (2)

//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------

uint8_t   PCA0CN_mask = 0xBF;
                                        // mask for bits: 00000001, see doc for PCA0CN
                                        // to check for the interrupt source is CCF0, 0th comparator
uint16_t  measurements[MAX_HISTORY]; // to store freq of max(uint16_t max) consecutive pulses to average
uint8_t   lastindex=0;            // index to freq array
//-----------------------------------------------------------------------------
// PCA0_Init
//-----------------------------------------------------------------------------
//
// Configure the Programmable Counters
//
void PCA0_Init()
{
    uint8_t SFRPAGE_SAVE = SFRPAGE;     // Save the current SFR page
    uint8_t modeN = 0x21;         // To detect positive edge triggering
                                        // 0x2- to enable capture on positive edge detected
                                        // 0x-1 to enable capture/compare interrupt


    SFRPAGE   = CONFIG_PAGE;            // set the SFR page to allow access to the necessary SFRs
    P0MDOUT  &= ~0x02;                  // Set P0.1 pin open drain
    P0       |=  0x02;                  // Set P0.1 pin output high to allow input
    // ** This crossbar change is compatible with Ethernet extension board AB_4 and CMX/Micronet TCP/IP stack **
    XBR0 = 0x08;                        // *****close CP0, Enable CEX0 of PCA0 I/O, Close UART0

    SFRPAGE   = PCA0_PAGE;              // set the SFR page to allow access to the necessary SFRs
    PCA0CN    = 0x00;                   // reset counter interrupt bits and disable the counter -0------
    PCA0MD    = 0x00;                   // ----SRC- controls the SouRCe of the counting
                                        //     000  - SYSCLK/12
                                        //     001  - SYSCLK/4
                                        //     010  - Timer0 overflow (but Timer0 will be used later for something)
                                        //     100  - SYSCK, also can use external inputs and external oscillator/8
                                        // -------1 - enables counter roll-over interrupt

    // program the 0th comparator for positive edge triggering
    PCA0CPM0  = modeN;   //using only Compare/Capture module 1 so PCA0CPM1 is only set to 0X20
    PCA0CPM1  = 0;
    PCA0CPM2  = 0;
    PCA0CPM3  = 0;
    PCA0CPM4  = 0;
    PCA0CPM5  = 0;

    // Important: set the low byte first (stops comparing), and the high byte next (starts comparing again)
    PCA0CPL0  = 0x00;
    PCA0CPH0  = 0x00;
    PCA0CPL1  = 0x00;
    PCA0CPH1  = 0x00;
    PCA0CPL2  = 0x00;
    PCA0CPH2  = 0x00;
    PCA0CPL3  = 0x00;
    PCA0CPH3  = 0x00;
    PCA0CPL4  = 0x00;
    PCA0CPH4  = 0x00;
    PCA0CPL5  = 0x00;
    PCA0CPH5  = 0x00;


    PCA0L     = 0x00;                   // Reset PCA Counter Value to 0x0000
    PCA0H     = 0x00;

    PCA0CN   |= 0x40;                   // enable the counter - -1------

    SFRPAGE   = CONFIG_PAGE;
    // ** This interrupt is not compatible with CMX/Micronet TCP/IP stack **
    EIE1     |= 0x08;                   // Enable PCA0 interrupt at EIE1.3

    SFRPAGE = SFRPAGE_SAVE;             // Restore the original SFR page
}

//-----------------------------------------------------------------------------
// PCA0_ISR -- Time Period Measurement
//-----------------------------------------------------------------------------
//
// This ISR is called on event in any of PCA0 comparators or PCA0 counter overflow.
// If we could output PCA0CN directly to P0.3-P0.6 then manual copying to P3
// would have been unnecessary. However, crossbar is set tu map /INT0 to P0.3
// that is used by Ethernet board and TCP/IP stack library later on
//
void PCA0_ISR (void) __interrupt 9  __using 3
{
    static uint16_t lastcapture;  // most recent reading
    static uint8_t  first_time=1; // first reading has no previous reading to subtract
    uint8_t intsrc;


    SFRPAGE = PCA0_PAGE;                // set the SFR page to allow access to the necessary SFRs
    intsrc  = PCA0CN & PCA0CN_mask;     // Check the interrupt source and mask it with action enable register
    PCA0CN  = 0x40;                     // We always must clear PCA0 interrupt flags manually!


    if (intsrc & 0x01)  //if +ve edge is detected
    {
        uint16_t currentcapture;

        // Note: must read PCA0CPL0 before PCA0CPH0
        currentcapture =  PCA0CPL0 | (PCA0CPH0 <<8);

        if (first_time)
        {
            first_time = 0;
        } else {
            ++lastindex; if (++lastindex==MAX_HISTORY) lastindex=0;
            measurements[lastindex] = currentcapture - lastcapture;
        }
        lastcapture=currentcapture;

     }
}

uint16_t getRecentPeriod()
{
    uint16_t res= 0;
    __bit EA_SAVE     = EA;             // Preserve the current Interrupt Status
    EA = 0;
    res = measurements[lastindex];
    EA = EA_SAVE;
    return(res);
}

uint16_t getAveragedPeriod()
{
    uint32_t sum = 0;
    uint8_t i;
    __bit EA_SAVE     = EA;             // Preserve the current Interrupt Status
    EA = 0;
    for (i=0; i<MAX_HISTORY; ++i)
        sum += measurements[i];
    EA = EA_SAVE;
    return( sum>>MAX_HISTDIV );
}
