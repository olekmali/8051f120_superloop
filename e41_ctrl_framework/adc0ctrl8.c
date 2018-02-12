#include "adc0ctrl8.h"

#include "C8051F120.h"
#include "C8051F120_io.h"

//-----------------------------------------------------------------------------
// Global VARIABLES
//-----------------------------------------------------------------------------

#define PLANT_MAX_ORDER (4)
static __xdata int16_t input[9][PLANT_MAX_ORDER];  // sampled 9 input channels values in circular buffer
static __xdata int16_t output[2][PLANT_MAX_ORDER]; // computer 2 output channels values in circular buffer
static __data uint8_t index = 0;

//------------------------------------------------------------------------------------
// getRecentResult
//------------------------------------------------------------------------------------
//
// This function returns the most recent conversion results
// Doing this in main function instead would save a few bytes of code
// However, putting it in a separate function results in code that is easier to follow
// and thus better both for firmware life cycle and educational purposes
//
uint16_t getRecentInput(uint8_t channel)
{
    char saveEA = EA;                   // save interrupt enabled status
    uint16_t returnvalue;
    EA = 0;                             // disable interrupts
    returnvalue = input[channel][index];
    EA = saveEA;                        // re-enable interrupts if were enabled
    return(returnvalue);
}


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
    int16_t counts = sysclock/12/rate;  // Init Timer3 to generate interrupts at a RATE Hz rate.
                                        // Note that timer3 is connected to SYSCLK/12

    SFRPAGE = TMR3_PAGE;                // set the SFR page to allow access to the necessary SFRs
    TMR3CN  = 0x00;                     // Stop Timer3; Clear TF3;
    TMR3CF  = 0x00;                     // use SYSCLK/12 as timebase
//  TMR3CF  = 0x08;                     // use SYSCLK as timebase
    RCAP3   = (uint16_t)(65536UL - counts);  // Set the timer reload value to ensure desired interrupt frequency
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
    SFRPAGE = DAC1_PAGE;                // set the SFR page to allow access to the necessary SFRs
    DAC1CN  = 0x8F;                     // enable DAC1 left justified, and set up to out on Timer3 overflow,
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
    SFRPAGE = TMR3_PAGE;                // set the SFR page to allow access to the necessary SFRs
    TF3 = 0;                            // clear TF3

    SFRPAGE = ADC0_PAGE;
    AMX0SL  = 0x00;                     // pick the first channel to read
    ADC0CF &= ~0x07;                    // reset gain to 1 (-----000)
    AD0INT  = 0;                        // enable the next conversion detection or interrupt
    AD0BUSY = 1;                        // take the channel measurement, start conversion

                                        // DACs are now updating their outputs
}

//------------------------------------------------------------------------------------
// ADC0_ISR
//------------------------------------------------------------------------------------
//
// ADC0 end-of-conversion ISR
//
void ADC0_ISR (void) __interrupt 15 __using 3
{

    LED = 0;                            // LED PWM must be way above 0% to ensure no missed interrupts


    if (AMX0SL<0x08)                    // if not done yet with all channels then
    {
        // read one of the data channels
        SFRPAGE = ADC0_PAGE;
        input[AMX0SL][index]= 0x8000 ^ ADC0; // read the meaqsurement for the current channel
        // 0x8000 ^ value adds a DC bias to make the rails 0 to 65535
        // Note: the XOR with 0x8000 translates the bipolar quantity into a unipolar quantity.
        AD0INT = 0;                     // clear ADC conversion complete indicator - must be done manually
        AMX0SL = (AMX0SL+1);            // pick the next channel to read - round robin
                                        // the roll over happens thanks to Timer3 overflow interrupt
                                        // ie.: 0, 1, ... 7, 8, 0, ...

        if (AMX0SL==0x08) ADC0CF|=0x01; // increase gain from 1 to 2 for the temperature sensor
        AD0BUSY = 1;                    // and start the conversion
    } else {                            // we have measurements for all channels and now we can compute the outputs

        // index variables to accesss delayed inputs and outputs
        // notation z1 meanse actually Z^-1 etc.
        uint8_t i0 = index;
        uint8_t i1 = (i0==0) ? PLANT_MAX_ORDER-1: i0-1 ;
//      uint8_t i2 = (i1==0) ? PLANT_MAX_ORDER-1: i1-1 ;
//      uint8_t i3 = (i2==0) ? PLANT_MAX_ORDER-1: i2-1 ;
//      uint8_t i4 = (i3==0) ? PLANT_MAX_ORDER-1: i3-1 ;

        int16_t X1n0=input[1][i0];          // X[1](n-0)
        int16_t X1n1=input[1][i1];          // X[1](n-1)
//      int16_t X1n2=input[1][i2];          // X[1](n-2)
//      int16_t X1n3=input[1][i3];          // X[1](n-3)
//      int16_t X1n4=input[1][i4];          // X[1](n-4)

        // we calculate Y1n0 that is y(n-0)
        int16_t Y1n1=output[1][i1];         // Y[1](n-1)
//      int16_t Y1n2=output[1][i2];         // Y[1](n-2)
//      int16_t Y1n3=output[1][i3];         // Y[1](n-3)
//      int16_t Y1n4=output[1][i4];         // Y[1](n-4)

        // read the temperature channel
        SFRPAGE = ADC0_PAGE;
        input[AMX0SL][index]=ADC0;      // read the meaqsurement for the current channel
        AD0INT = 0;                     // clear ADC conversion complete indicator - must be done manually

        /* implement the controller below this line */

            // remember that we have time only for simple integer arithmetic (or fixed point decimal)    
    
            // testing - pass through
            output[0][i0] = (int16_t)(  input[0][i0] );
            
            // plant control for dt=1E-3: y(n) = -(10.0/14)*y(n-1) +0.83*x(n) - (0.83*11/14)*x(n-1)
            output[1][i0] = (int16_t)(  ( - 10L*Y1n1 + (83*14L*X1n0 - 83*11L*X1n1)/100 )/14  );
    
    
    
        /* implement the controller above this line */
    }
    SFRPAGE = DAC0_PAGE;                // set the SFR page to allow access to the necessary SFRs
    DAC0 = 0x8000 ^ output[0][index];
    // 0x8000 ^ value adds a DC bias to make the rails 0 to 65535
    // Note: the XOR with 0x8000 translates the bipolar quantity into a unipolar quantity.
    SFRPAGE = DAC1_PAGE;            // set the SFR page to allow access to the necessary SFRs
    DAC1 = 0x8000 ^ output[1][index];
    // 0x8000 ^ value adds a DC bias to make the rails 0 to 65535
    // Note: the XOR with 0x8000 translates the bipolar quantity into a unipolar quantity.

    index++; if (index>=PLANT_MAX_ORDER) index=0;     // roll over the buffer as necessary

    if (!SW2) LED = 1;                  // debug timing only when SW2 is pressed (to reduce noise)
}
