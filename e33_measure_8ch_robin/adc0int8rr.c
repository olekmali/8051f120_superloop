#include "adc0int8rr.h"

#include "C8051F120.h"                  // SFR declarations

//-----------------------------------------------------------------------------
// Global VARIABLES
//-----------------------------------------------------------------------------

uint16_t result[9];                 // ADC0 decimated value - visible outside by means
                                        // of defining it as external in the library header

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
void ADC0_Timer3_Init (uint32_t sysclock, uint32_t rate)
{
    uint8_t SFRPAGE_SAVE = SFRPAGE;        // Save Current SFR page
    int16_t counts = sysclock/rate;         // Init Timer3 to generate interrupts at a RATE Hz rate.
                                        // Note that timer3 is connected to SYSCLK

    SFRPAGE = TMR3_PAGE;                // set SFR page
    TMR3CN  = 0x00;                     // Stop Timer3; Clear TF3;
//  TMR3CF  = 0x00;                     // use SYSCLK/12 as timebase
    TMR3CF  = 0x08;                     // use SYSCLK as timebase
    RCAP3   = 65536 -(uint16_t)counts;  // Init reload values
    // or   = -(uint16_t)counts; -- see the in class comment
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
// We post the result in the global variable <result> and round robin switch to the next channel.
//
void ADC0_ISR (void) __interrupt 15
{
    int16_t ch;
    SFRPAGE = ADC0_PAGE;
    ch = AMX0SL;                        // retrieve which channel has just been measured
    result[ch] = ADC0;                  // read ADC value from the previously selected channel
    
    ch++; if (ch>8) ch=0;
    AMX0SL = ch;                        // pick the next channel to read - round robin 0, 1, ... 8, 0, ...
    AD0INT = 0;                         // clear ADC conversion complete indicator - must be done manually
}


//------------------------------------------------------------------------------------
// setGain
//------------------------------------------------------------------------------------
//
// Change gain for all channels of ADC0
//
void setGain(uint8_t gain)
{
/*
    -----000 => *1
    -----001 => *2
    -----010 => *4
    -----011 => *8
    -----10x => *16
    -----11x => /2
*/
    uint8_t SFRPAGE_SAVE = SFRPAGE;        // Save Current SFR page
    uint8_t saveEA = EA;          // save interrupt enabled status

    uint8_t encoded=0;
    while(gain>1)
    {
        encoded++;
        gain=gain>>1;                   // gain/=2;
    }

    EA = 0;                             // disable interrupts
    ADC0CF = (ADC0CF & 0xF8) | ( encoded & 0x07); // set the gain
    EA = saveEA;                        // re-enable interrupts if were enabled
    SFRPAGE = SFRPAGE_SAVE;             // Restore SFR page
}


//------------------------------------------------------------------------------------
// getRecentResult
//------------------------------------------------------------------------------------
//
// This function returns the most recent conversion results
// Doing this in main function instead would save a few bytes of code
// However, putting it in a separate function results in code that is easier to follow
// and thus better both for firmware life cycle and educational purposes
//
uint16_t getRecentResult(uint8_t channel)
{
    char saveEA = EA;                   // save interrupt enabled status
    uint16_t returnvalue;
    EA = 0;                             // disable interrupts
    returnvalue = result[channel];
    EA = saveEA;                        // re-enable interrupts if were enabled
    return(returnvalue);
}
