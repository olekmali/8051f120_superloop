#include "adc0wait.h"

#include "C8051F120.h"

//------------------------------------------------------------------------------------
// ADC0_Wait_Init
//------------------------------------------------------------------------------------
//
// Configure ADC0 to use SFR as conversion source, to
// generate an interrupt on conversion complete, and to use left-justified
// output mode.  Enables ADC end of conversion interrupt. Leaves ADC disabled.
//
void ADC0_Wait_Init(uint32_t sysclock)
{
    uint8_t SFRPAGE_SAVE = SFRPAGE;     // Save the current SFR page

    SFRPAGE = ADC0_PAGE;
    // ADC0 Control
    ADC0CN = 0x41;                      // ADC0 disabled; normal tracking mode; data is left-justified
                                        // 0x00 ADC0 conversions are initiated manually by setting AD0BUSY=1
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

    // EIE2 |= 0x02;                    // enable ADC interrupts when the conversion is complete - not needed
    AD0EN = 1;                          // enable ADC
 
    SFRPAGE = SFRPAGE_SAVE;             // Restore the original SFR page
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
    uint8_t SFRPAGE_SAVE = SFRPAGE;     // Save the current SFR page

    uint8_t encoded=0;
    while(gain>1)
    {
        encoded++;
        gain = gain>>1;                 // gain=gain/2;
    }

    SFRPAGE = ADC0_PAGE;
    ADC0CF = (ADC0CF & 0xF8) | ( encoded & 0x07);

    SFRPAGE = SFRPAGE_SAVE;             // Restore the original SFR page
}


//------------------------------------------------------------------------------------
// setChannel
//------------------------------------------------------------------------------------
//
// Change the input channel of ADC0
//
void setChannel(uint8_t channel)
{
    uint8_t SFRPAGE_SAVE = SFRPAGE;     // Save the current SFR page
    SFRPAGE = ADC0_PAGE;

    AMX0SL = channel;

    SFRPAGE = SFRPAGE_SAVE;             // Restore the original SFR page
}


//------------------------------------------------------------------------------------
// getADC0
//------------------------------------------------------------------------------------
//
// Sets up taking a sample, and waits for the result
//
uint16_t getADC0()
{
    uint8_t SFRPAGE_SAVE = SFRPAGE;     // Save the current SFR page
    uint16_t result;

    SFRPAGE = ADC0_PAGE;

    AD0INT  = 0;                        // enable the next conversion detection or interrupt
    AD0BUSY = 1;                        // take measurement, start conversion
                                        // AD0BUSY used on SiLabs reference design, AD0INT in data sheets
//  while (AD0BUSY) ;                   // never do it this way! - exits prematurely!!!
    while (!AD0INT) ;                   // wait until it is complete (at least 0.00001 sec)

    result = ADC0;

    SFRPAGE = SFRPAGE_SAVE;             // Restore the original SFR page
    return(result);
}
