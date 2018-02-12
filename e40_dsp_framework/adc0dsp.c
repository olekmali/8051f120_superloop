#include "adc0dsp.h"

#include "C8051F120.h"
#include "C8051F120_io.h"

//------------------------------------------------------------------------------------
// ADC0_DACs_Timer3_Init
//------------------------------------------------------------------------------------
//
// Configure Timer3 to auto-reload and generate an interrupt at interval
// specified by <counts> using SYSCLK/12 as its time base.
//
// Configure ADC0 to use Timer3 overflows as conversion source, to
// generate an interrupt on conversion complete, and to use left-justified
// output mode.  Enables ADC end of conversion interrupt. Leaves ADC disabled.
//
void ADC0_DACs_Timer3_Init (uint32_t sysclock, uint32_t rate)
{
    uint8_t SFRPAGE_SAVE = SFRPAGE;     // Save the current SFR page
    uint16_t counts = (uint16_t)( sysclock/(12UL*rate) );      // Init Timer3 to generate interrupts at a RATE Hz rate.
                                        // Note that timer3 is connected to SYSCLK/12

    SFRPAGE = TMR3_PAGE;                // set the SFR page to allow access to the necessary SFRs
    TMR3CN  = 0x00;                     // Stop Timer3; Clear TF3;
    TMR3CF  = 0x00;                     // use SYSCLK/12 as timebase
//  TMR3CF  = 0x08;                     // use SYSCLK as timebase
    RCAP3   = (uint16_t)(65536UL - counts); // Set the timer reload value to ensure the desired interrupt frequency
    // or   = -counts; -- see the in class comment
    TMR3    = RCAP3;                    // set to reload immediately
    EIE2   &= ~0x01;                    // DISABLE Timer3 interrupts, or simply don't enable them
    TMR3CN |= 0x04;                     // start Timer3


    SFRPAGE = ADC0_PAGE;
    // ADC0 Control
    ADC0CN = 0x05;                      // ADC0 disabled; normal tracking mode; data is left-justified
                                        // 0x04 ADC0 conversions are initiated on overflow of Timer3
                                        // ----00-- ADC0 conversions started manually by setting AD0BUSY=1
                                        // ----01-- ADC0 conversions are initiated on overflow of Timer3
                                        // ----11-- ADC0 conversions are initiated on overflow of Timer2
                                        // 0x01 ADC0 data is left-justified
                                        // 0x40 ADC0 in low power tracking mode - adds 1.5us wait before conversion starts
                                        // 0x40 is not needed in this case because we will change the multiplexer source
                                        //      when a conversion is done and let the conversion begin at the next Timer3
                                        //      rolls over - that should meet this wait time requirement
                                        //      as int32_t as the sampling frequency is way below 100KHz

    // Conversion speed and input gain
    ADC0CF = (sysclock / 2500000) << 3; // ddddd--- ADC0 conversion clock at 2.5 MHz
    ADC0CF |= 0x00;                     // -----ddd ADC) internal gain (PGA): 0-1, 1-2, 2-4, 3-8, 4-16, 6-0.5

    // Voltage Source and mode
    REF0CN = 0x07;                      // enable: 0x01 on-chip VREF, 0x02 VREF output buffer for ADC and DAC, and 0x04 temp sensor
    AMX0CF = 0x00;                      // 0x00 Select 8 independent inputs, 0x0F select 4 differential pairs
    AMX0SL = 0x00;                      // Select Channel 0 as ADC mux output,
                                        // When AMX0CF = 0x00 then AMX0SL = channel number, 8 for TEMP sensor

    EIE2 |= 0x02;                       // enable ADC interrupts when the conversion is complete
    AD0EN = 1;                          // enable ADC
    // Remember to enable global interrupts when ready


    SFRPAGE = DAC0_PAGE;                // set the SFR page to allow access to the necessary SFRs
//  REF0CN |= 0x07;                     // enable: 0x01 on-chip VREF, 0x02 VREF output buffer for ADC and DAC, and 0x04 temp sensor
    DAC0CN  = 0x8F;                     // enable DAC0 left justified, and set up to out on Timer3 overflow,
                                        // 1------- enable                     <-- this one is used
                                        // -----000 right justify 12bit input
                                        // -----1** left  justify 12bit input  <-- this one is used
                                        // ---00--- execute on loading DAC0H
                                        // ---01--- execute on Timer3 overflow <-- this one is used
                                        // ---10--- execute on Timer4 overflow
                                        // ---11--- execute on Timer2 overflow


    SFRPAGE = SFRPAGE_SAVE;             // Restore the original SFR page
}

//------------------------------------------------------------------------------------
// Internal buffer
//------------------------------------------------------------------------------------

#define BUFLEN (10)
// Note: use __xdata in case BUFLEN > 10 to save fast memory
static __data int16_t bufinp[BUFLEN] = {0}; // circular data buffer
static __data int16_t bufout[BUFLEN] = {0}; // circular data buffer
static __data  uint8_t bufndx= 0; // current index

//------------------------------------------------------------------------------------
// Interrupt Service Routines
//------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------
// ADC0_ISR
//------------------------------------------------------------------------------------
//
// ADC0 end-of-conversion ISR
//
void ADC0_ISR (void) __interrupt 15 __using 3
{
    LED = 0;                            // LED PWM must be way above 0% to ensure no missed interrupts

    SFRPAGE = ADC0_PAGE;
    bufinp[bufndx] = 0x8000 ^ ADC0;     // read ADC value from the previously selected channel
    AD0INT = 0;                         // clear ADC conversion complete indicator - must be done manually

    {
        // index variables to access delayed inputs and outputs
        // notation z1 means actually Z^-1 etc.
        uint8_t i0 = bufndx;
        uint8_t i1 = (i0==0) ? BUFLEN-1: i0-1 ;
        uint8_t i2 = (i1==0) ? BUFLEN-1: i1-1 ;
        uint8_t i3 = (i2==0) ? BUFLEN-1: i2-1 ;
        uint8_t i4 = (i3==0) ? BUFLEN-1: i3-1 ;
        uint8_t i5 = (i4==0) ? BUFLEN-1: i4-1 ;
        uint8_t i6 = (i5==0) ? BUFLEN-1: i5-1 ;
        uint8_t i7 = (i6==0) ? BUFLEN-1: i6-1 ;
        uint8_t i8 = (i7==0) ? BUFLEN-1: i7-1 ;
//      uint8_t i9 = (i8==0) ? BUFLEN-1: i8-1 ;


    /* implement the filter below this line */
        // remember that we have time only for simple integer arithmetic (or fixed point decimal)

        /* for example see: http://www-users.cs.york.ac.uk/~fisher/mkfilter/trad.html
            Wished parameters:
                  filtertype 	= 	Butterworth
                  passtype 	= 	Bandpass
                  ripple 	=
                  order 	= 	4
                  samplerate 	= 	8000
                  corner1 	= 	500
                  corner2 	= 	1500
                  adzero 	=
                  logmin 	=
              Experimental results (damaged due to sensitivity of coefficients):
                  corner1 	= 	1500
                  corner2 	= 	2500
        */

        bufout[i0] = (int16_t)(
            (
                  (  1000L * bufinp[i8])
                + (     0L * bufinp[i7])
                + ( -4000L * bufinp[i6])
                + (     0L * bufinp[i5])
                + (  6000L * bufinp[i4])
                + (     0L * bufinp[i3])
                + ( -4000L * bufinp[i2])
                + (     0L * bufinp[i1])
                + (  1000L * bufinp[i0])
                + (  -120L/*.3895999*/ * bufout[i8])
                + (   923L/*.0548098*/ * bufout[i7])
                + ( -3437L/*.6056188*/ * bufout[i6])
                + (  7949L/*.9769608*/ * bufout[i5])
                + (-12439L/*.7926885*/ * bufout[i4])
                + ( 13499L/*.1258864*/ * bufout[i3])
                + ( -9959L/*.2249769*/ * bufout[i2])
                + (  4568L/*.0368627*/ * bufout[i1])
            ) / 97948                   // actually / (gain* 1000)
        );


    /* implement the filter above this line */
    }

    SFRPAGE = DAC0_PAGE;                // set the SFR page to allow access to the necessary SFRs
    DAC0 = 0x8000 ^ bufout[bufndx];
    // 0x8000 ^ value adds a DC bias to make the rails 0 to 65535
    // Note: the XOR with 0x8000 translates the bipolar quantity into a unipolar quantity.

    bufndx++; if (bufndx==BUFLEN) bufndx=0;

    if (!SW2) LED = 1;                  // debug timing only when SW2 is pressed (to reduce noise)
}
