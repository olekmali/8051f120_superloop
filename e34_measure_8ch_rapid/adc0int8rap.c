#include "adc0int8rap.h"

#include "C8051F120.h"                  // SFR declarations

//-----------------------------------------------------------------------------
// Global VARIABLES
//-----------------------------------------------------------------------------

static uint16_t result[(ADC0_CH_MAX+1)];   // ADC0 captured values
static uint8_t nextchannel = 0;          // next channel to read
static uint8_t timing=0;                 // a crude semaphore 


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
        gain = gain>>1;                 // gain=gain/2;
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


//------------------------------------------------------------------------------------
// is_data_ready is a simple semaphore take
//------------------------------------------------------------------------------------
//
uint8_t is_data_ready()
{
    char saveEA = EA;                   // save interrupt enabled status
    uint8_t returnvalue;
    EA = 0;                             // disable interrupts
    returnvalue = timing;               // or use  return value = (timing==1); for binary output of false/true aka 0/1
    EA = saveEA;                        // re-enable interrupts if were enabled
    return(returnvalue);
}


//------------------------------------------------------------------------------------
// reset_data_ready is a simple semaphore give
//------------------------------------------------------------------------------------
//
void reset_data_ready()
{
    char saveEA = EA;                   // save interrupt enabled status
    EA = 0;                             // disable interrupts
    timing = 0;
    EA = saveEA;                        // re-enable interrupts if were enabled
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
void ADC0_Timer3_Init (uint32_t sysclock, uint32_t rate)
{
    uint8_t SFRPAGE_SAVE = SFRPAGE;        // Save Current SFR page
    int16_t counts = sysclock/12/rate;      // Init Timer3 to generate interrupts at a RATE Hz rate.
                                        // Note that timer3 is connected to SYSCLK/12

    SFRPAGE = TMR3_PAGE;                // set SFR page
    TMR3CF  = 0x00;                     // use SYSCLK/12 as timebase
//  TMR3CF  = 0x08;                     // use SYSCLK as timebase
    RCAP3   = 65536 -(uint16_t)counts;  // Init reload values
    // or   = -(uint16_t)counts; -- see the in class comment
    TMR3    = RCAP3;                    // set to reload immediately
    EIE2   |= 0x01;                     // ENABLE Timer3 interrupts
    TMR3CN |= 0x04;                     // start Timer3

    SFRPAGE = ADC0_PAGE;
    // ADC0 Control
    ADC0CN = 0x41;                      // ADC0 disabled; LOW POWER tracking mode; data is left-justified
                                        // 0x41 ADC0 conversions are initiated manually
                                        // ----00-- ADC0 conversions started manually by setting AD0BUSY=1
                                        // ----01-- ADC0 conversions are initiated on overflow of Timer3
                                        // ----11-- ADC0 conversions are initiated on overflow of Timer2
                                        // 0x01 ADC0 data is left-justified
                                        // 0x40 ADC0 in low power tracking mode - adds 1.5us wait before conversion starts
                                        // 0x40 IS NEEDED in this case because we will change the multiplexer source
                                        //      just before requesting the next reading - need to have setting time

    // Conversion speed and input gain
    ADC0CF = (sysclock / 2500000) << 3; // ddddd--- ADC0 conversion clock at 2.5 MHz
    ADC0CF |= 0x00;                     // -----ddd ADC) internal gain (PGA): 0-1, 1-2, 2-4, 3-8, 4-16, 6-0.5
                                        // Temperature sensor works with highest range and resolution with gain of 2

    // Voltage Source and mode
    REF0CN = 0x07;                      // enable: 0x01 on-chip VREF, 0x02 VREF output buffer for ADC and DAC, and 0x04 temp sensor
    AMX0CF = 0x00;                      // 0x00 Select 8 independent inputs, 0x0F select 4 differential pairs
    AMX0SL = 0x00;                      // Select TEMP sensor as ADC mux output, 0x00-0x07 for 8 inputs
                                        // When AMX0CF = 0x00 then AMX0SL = channel number, 8 for TEMP sensor

    EIE2 |= 0x02;                       // enable ADC interrupts when the conversion is complete
    AD0EN = 1;                          // enable ADC
    // Remember to enable global interrupts when ready

    SFRPAGE = SFRPAGE_SAVE;             // Restore SFR page
}


//------------------------------------------------------------------------------------
// Interrupt Service Routines
//------------------------------------------------------------------------------------

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
    SFRPAGE = TMR3_PAGE;                // set SFR page
    TF3 = 0;                            // clear TF3

    nextchannel = 0;

    SFRPAGE = ADC0_PAGE;
    AMX0SL  = 0;                        // next channel to read
    AD0INT  = 0;                        // enable the next conversion detection or interrupt
    AD0BUSY = 1;                        // take the channel measurement, start conversion

                                        // DACs are now updating their outputs
}

//------------------------------------------------------------------------------------
// ADC0_ISR
//------------------------------------------------------------------------------------
//
// ADC0 end-of-conversion ISR
// We post the result in the global variable <result> and round robin switch to the next channel.
//
void ADC0_ISR (void) __interrupt 15
{
    SFRPAGE = ADC0_PAGE;
    AD0INT  = 0;

    result[nextchannel] = ADC0;         // read ADC value from the previously selected channel
    AD0INT  = 0;                        // clear ADC conversion complete indicator - must be done manually
    if (nextchannel<ADC0_CH_MAX)        // if not done yet with all channels then
    {
        // read one of the data channels
        nextchannel++;
        AMX0SL  = nextchannel;          // pick the next channel to read
        AD0BUSY = 1;                    // and start the conversion
    } else {
    // we do not start another conversion, it will be started by the next Timer3 overflow interrupt routine
        timing = 1;                     // all data is now ready for use
    }
}
