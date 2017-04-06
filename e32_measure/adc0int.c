#include "adc0int.h"

#include "C8051F120.h"                  // SFR declarations

//-----------------------------------------------------------------------------
// Global VARIABLES
//-----------------------------------------------------------------------------

uint16_t result;                        // ADC0 decimated value - visible outside by means
                                        // of defining it as external in the library header
uint16_t avgcnt_cnt;                    // how many readings should be averaged for accuracy
uint8_t  avgcnt_rct;                    // how many readings should be averaged for accuracy

//------------------------------------------------------------------------------------
// getRecentResult
//------------------------------------------------------------------------------------
//
// This function returns the most recent conversion results
// Doing this in main function instead would save a few bytes of code
// However, putting it in a separate function results in code that is easier to follow
// and thus better both for firmware life cycle and educational purposes
//
uint16_t getRecentResult()
{
    char saveEA = EA;                   // save interrupt enabled status
    uint16_t returnvalue;
    EA = 0;                             // disable interrupts
    returnvalue = result;
    EA = saveEA;                        // re-enable interrupts if were enabled
    return(returnvalue);
}


//------------------------------------------------------------------------------------
// ADC0_Timer3_Init
//------------------------------------------------------------------------------------
//
// Configure Timer3 to auto-reload and generate an interrupt at interval
// specified by <counts> using SYSCLK/12 as its time base.
//
// Configure ADC0 to use Timer3 overflows as conversion source, to
// generate an interrupt on conversion complete, and to use left-justified
// output mode.  Enables ADC end of conversion interrupt. Leaves interrupts disabled.
//
void ADC0_Timer3_Init (uint32_t sysclock, uint32_t rate, uint8_t avgfactor)
{
    uint8_t SFRPAGE_SAVE = SFRPAGE;        // Save Current SFR page
    uint16_t counts = (uint16_t)( sysclock/rate ); // Init Timer3 to generate interrupts at a RATE Hz rate.
                                        // Note that timer3 is connected to SYSCLK
    avgcnt_rct = avgfactor;
    avgcnt_cnt = 1<<avgcnt_rct;

    SFRPAGE = TMR3_PAGE;                // set SFR page
    TMR3CN  = 0x00;                     // Stop Timer3; Clear TF3;
//  TMR3CF  = 0x00;                     // use SYSCLK/12 as timebase
    TMR3CF  = 0x08;                     // use SYSCLK as timebase
    RCAP3   = (uint16_t)( 65536U - counts ); // Init reload values
    // or   = -counts; -- see the in class comment
    TMR3    = RCAP3;                    // set to reload immediately
    EIE2   &= ~0x01;                    // DISABLE Timer3 interrupts
    TMR3CN |= 0x04;                     // start Timer3

    SFRPAGE = ADC0_PAGE;
    // ADC0 Control
    ADC0CN = 0x05;                      // ADC0 disabled; normal tracking mode; data is left-justified
                                        // 0x04 ADC0 conversions are initiated on overflow of Timer3
                                        // ----00-- ADC0 conversions started manually by setting AD0BUSY=1
                                        // ----01-- ADC0 conversions are initiated on overflow of Timer3
                                        // ----11-- ADC0 conversions are initiated on overflow of Timer2
                                        // 0x01 ADC0 data is left-justified

    // Conversion speed and input gain
    ADC0CF = (sysclock / 2500000) << 3; // ddddd--- ADC0 conversion clock at 2.5 MHz
    ADC0CF |= 0x01;                     // -----ddd ADC) internal gain (PGA): 0-1, 1-2, 2-4, 3-8, 4-16, 6-0.5
                                        // Temperature sensor works with highest range and resolution with gain of 2

    // Voltage Source and mode
    REF0CN = 0x07;                      // enable: 0x01 on-chip VREF, 0x02 VREF output buffer for ADC and DAC, and 0x04 temp sensor
    AMX0CF = 0x00;                      // 0x00 Select 8 independent inputs, 0x0F select 4 differential pairs
    AMX0SL = 0x08;                      // Select TEMP sensor as ADC mux output, 0x00-0x07 for 8 inputs
                                        // When AMX0CF = 0x00 then AMX0SL = channel number, 8 for TEMP sensor


    // EIE2 |= 0x01;                    // enable Timer3 interrupts - not doing that this time, instead/in addition to we must: 
    EIE2 |= 0x02;                       // enable ADC interrupts when the conversion is complete
    AD0EN = 1;                          // enable ADC
    // Remember to enable global interrupts when ready

    SFRPAGE = SFRPAGE_SAVE;             // Restore SFR page
}


//------------------------------------------------------------------------------------
// Interrupt Service Routines
//------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------
// ADC0_ISR
//------------------------------------------------------------------------------------
//
// ADC0 end-of-conversion ISR
// Here we take the ADC0 sample, add it to a running total <sumforaverage>, and
// decrement our local decimation counter <sum_cnt>.  When <sum_cnt> reaches
// zero, we post the decimated result in the global variable <result>.
//
void ADC0_ISR (void) __interrupt 15
{
    static uint16_t sum_cnt = 1;    // averaging counter counter
                                        // we post a new result when sum_cnt = 0
                                        // initial =1 so the first result is bogus
    static int32_t sumforaverage = 0L; // here's where we integrate the ADC samples

    sumforaverage += ADC0;              // read ADC value and add to running total
    sum_cnt--;                          // update decimation counter
    AD0INT = 0;                         // clear ADC conversion complete indicator

    if (sum_cnt == 0) {                 // if zero, then post result
        sum_cnt = avgcnt_cnt;           // reset counter

        /* We will do a hocus-pocus instead of /avgcnt_rct because we are inside an interrupt
           and int32_t/int16_t may not be reentrant in your compiler. Besides shifting is quicker.
           The only drawback is that we can average by /a_power_of_two_number */

        result = sumforaverage >> avgcnt_rct; // update the result
        sumforaverage = 0L;             // reset sumforaverage
    }
}
