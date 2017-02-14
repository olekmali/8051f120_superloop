#include "dac0int.h"

#include "C8051F120.h"                  // SFR declarations
#include "C8051F120_io.h"               // SFR declarations


//------------------------------------------------------------------------------------
// Global CONSTANTS
//------------------------------------------------------------------------------------
#define PHASE_PREC (65535)              // range of phase accumulator
#define PHASE_HALF (32767)              // half of range of phase accumulator

//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------

Waveform output_waveform = OFF;
uint32_t  sampling    = 100000;        // sampling frequency of output in Hz, defaults to 100 kHz
uint16_t frequency   = 1000;           // frequency of output in Hz, defaults to 1000 Hz
uint16_t amplitude   = PHASE_PREC;
uint16_t offset      = PHASE_PREC>>1;
uint16_t phase_add   = (uint16_t)(628L * 1000L * 65536 / 100000L / 100); // for 1kHz signal and 100kHz sampling rate

// a full cycle, 16-bit, 2's complement sine wave lookup table
__code const int16_t SINE_TABLE[128] = { // half - 128, full - 256

    0x0000, 0x0324, 0x0647, 0x096a, 0x0c8b, 0x0fab, 0x12c8, 0x15e2,
    0x18f8, 0x1c0b, 0x1f19, 0x2223, 0x2528, 0x2826, 0x2b1f, 0x2e11,
    0x30fb, 0x33de, 0x36ba, 0x398c, 0x3c56, 0x3f17, 0x41ce, 0x447a,
    0x471c, 0x49b4, 0x4c3f, 0x4ebf, 0x5133, 0x539b, 0x55f5, 0x5842,
    0x5a82, 0x5cb4, 0x5ed7, 0x60ec, 0x62f2, 0x64e8, 0x66cf, 0x68a6,
    0x6a6d, 0x6c24, 0x6dca, 0x6f5f, 0x70e2, 0x7255, 0x73b5, 0x7504,
    0x7641, 0x776c, 0x7884, 0x798a, 0x7a7d, 0x7b5d, 0x7c29, 0x7ce3,
    0x7d8a, 0x7e1d, 0x7e9d, 0x7f09, 0x7f62, 0x7fa7, 0x7fd8, 0x7ff6,
    0x7fff, 0x7ff6, 0x7fd8, 0x7fa7, 0x7f62, 0x7f09, 0x7e9d, 0x7e1d,
    0x7d8a, 0x7ce3, 0x7c29, 0x7b5d, 0x7a7d, 0x798a, 0x7884, 0x776c,
    0x7641, 0x7504, 0x73b5, 0x7255, 0x70e2, 0x6f5f, 0x6dca, 0x6c24,
    0x6a6d, 0x68a6, 0x66cf, 0x64e8, 0x62f2, 0x60ec, 0x5ed7, 0x5cb4,
    0x5a82, 0x5842, 0x55f5, 0x539b, 0x5133, 0x4ebf, 0x4c3f, 0x49b4,
    0x471c, 0x447a, 0x41ce, 0x3f17, 0x3c56, 0x398c, 0x36ba, 0x33de,
    0x30fb, 0x2e11, 0x2b1f, 0x2826, 0x2528, 0x2223, 0x1f19, 0x1c0b,
    0x18f8, 0x15e2, 0x12c8, 0x0fab, 0x0c8b, 0x096a, 0x0647, 0x0324,
/*
    0x0000, 0xfcdc, 0xf9b9, 0xf696, 0xf375, 0xf055, 0xed38, 0xea1e,
    0xe708, 0xe3f5, 0xe0e7, 0xdddd, 0xdad8, 0xd7da, 0xd4e1, 0xd1ef,
    0xcf05, 0xcc22, 0xc946, 0xc674, 0xc3aa, 0xc0e9, 0xbe32, 0xbb86,
    0xb8e4, 0xb64c, 0xb3c1, 0xb141, 0xaecd, 0xac65, 0xaa0b, 0xa7be,
    0xa57e, 0xa34c, 0xa129, 0x9f14, 0x9d0e, 0x9b18, 0x9931, 0x975a,
    0x9593, 0x93dc, 0x9236, 0x90a1, 0x8f1e, 0x8dab, 0x8c4b, 0x8afc,
    0x89bf, 0x8894, 0x877c, 0x8676, 0x8583, 0x84a3, 0x83d7, 0x831d,
    0x8276, 0x81e3, 0x8163, 0x80f7, 0x809e, 0x8059, 0x8028, 0x800a,
    0x8000, 0x800a, 0x8028, 0x8059, 0x809e, 0x80f7, 0x8163, 0x81e3,
    0x8276, 0x831d, 0x83d7, 0x84a3, 0x8583, 0x8676, 0x877c, 0x8894,
    0x89bf, 0x8afc, 0x8c4b, 0x8dab, 0x8f1e, 0x90a1, 0x9236, 0x93dc,
    0x9593, 0x975a, 0x9931, 0x9b18, 0x9d0e, 0x9f14, 0xa129, 0xa34c,
    0xa57e, 0xa7be, 0xaa0b, 0xac65, 0xaecd, 0xb141, 0xb3c1, 0xb64c,
    0xb8e4, 0xbb86, 0xbe32, 0xc0e9, 0xc3aa, 0xc674, 0xc946, 0xcc22,
    0xcf05, 0xd1ef, 0xd4e1, 0xd7da, 0xdad8, 0xdddd, 0xe0e7, 0xe3f5,
    0xe708, 0xea1e, 0xed38, 0xf055, 0xf375, 0xf696, 0xf9b9, 0xfcdc, */
};


//------------------------------------------------------------------------------------
// ADC0_Timer3_Init
//------------------------------------------------------------------------------------
//
// Configure Timer4 to auto-reload and generate an interrupt at interval
// specified by <counts> using SYSCLK/12 as its time base.
//
void DAC0_Timer3_Init (uint32_t sysclock, uint32_t rate)
{
    uint8_t SFRPAGE_SAVE = SFRPAGE;        // Save Current SFR page
    uint32_t counts = sysclock/12/rate; // Note that timer3 is connected to SYSCLK/12

    sampling = rate;
    phase_add  = (uint16_t)((628L * frequency * 65536) / sampling / 100);

    SFRPAGE = DAC0_PAGE;                // set SFR page
    REF0CN  = 0x03;                     // enable: 0x01 on-chip VREF, 0x02 VREF output buffer for ADC and DAC, and 0x04 temp sensor
    DAC0CN  = 0x8F;                     // enable DAC0 left justified, and set up to out on Timer4 overflow, 
                                        // 1------- enable
                                        // -----000 right justify 12bit input
                                        // -----1** left justify 12bit input
                                        // ---00--- execute on loading DAC0H
                                        // ---01--- execute on Timer3 overflow
                                        // ---10--- execute on Timer4 overflow 
                                        // ---11--- execute on Timer2 overflow

    SFRPAGE = TMR3_PAGE;                // set SFR page
    TMR3CN  = 0x00;                     // Stop Timer3; Clear TF3;
    TMR3CF  = 0x00;                     // use SYSCLK/12 as timebase
//  TMR3CF  = 0x08;                     // use SYSCLK as timebase
    RCAP3   = 65536 -(uint16_t)counts;  // Init reload values
    // or   = -(uint16_t)counts; -- see the in class comment
    TMR3    = 0xffff;                   // set to reload immediately
    EIE2   |= 0x01;                     // enable Timer3 interrupts - bit 00000001 or ET3 = 1;
    TMR3CN |= 0x04;                     // start Timer3

    SFRPAGE = SFRPAGE_SAVE;             // Restore SFR page
}

//------------------------------------------------------------------------------------
// Parameter Control Functions
//------------------------------------------------------------------------------------

void DAC0_Timer3_SetType(Waveform newtype)
{
    output_waveform = newtype;
}

void DAC0_Timer3_SetFrequency(uint16_t newfrequency)
{
    __bit EA_SAVE     = EA;             // Preserve Current Interrupt Status
    EA = 0;                             // disable interrupts
    if (newfrequency<1) newfrequency=1;
    frequency = newfrequency;
    phase_add  = (uint16_t)((628L * frequency * 65536) / sampling / 100);
    EA = EA_SAVE;                       // restore interrupts
}

void DAC0_Timer3_SetAmplitude(uint8_t newamplitude)
{
    __bit EA_SAVE     = EA;             // Preserve Current Interrupt Status
    EA = 0;                             // disable interrupts
    if (newamplitude>100) newamplitude=100;
    amplitude = (uint32_t)PHASE_PREC * newamplitude / 100;
    EA = EA_SAVE;                       // restore interrupts
}

void DAC0_Timer3_SetOffset(uint8_t newoffset)
{
    __bit EA_SAVE     = EA;             // Preserve Current Interrupt Status
    EA = 0;                             // disable interrupts
    if (newoffset>100) newoffset=100;
    offset = (uint32_t)PHASE_PREC * newoffset / 100;
    EA = EA_SAVE;                       // restore interrupts
}

//------------------------------------------------------------------------------------
// Interrupt Service Routines
//------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// DAC0_Timer3_ISR -- Wave Generator
//-----------------------------------------------------------------------------
//
// This ISR is called on Timer4 overflows.  Timer4 is set to auto-reload mode
// and is used to schedule the DAC output sample rate in this example.
// Note that the value that is written to DAC0 during this ISR call is
// actually transferred to DAC0 at the next Timer4 overflow.
//
void DAC0_Timer3_ISR (void) __interrupt 14 // __using 3
{
    __code int16_t *table_ptr=SINE_TABLE;       // pointer in data memory to the lookup table located in code memory
    static uint16_t phase_acc = 0;              // holds phase accumulator, Note: will roll over at 65536

    int16_t  temp1;                             // the temporary value that passes
                                                // through 3 stages before being written to DAC1
    int32_t temp2;                              // temp1 adjusted with current amplitude

    // we are on TMR3_PAGE page right now
    // SFRPAGE = TMR3_PAGE;                     // set SFR page
    TF3 = 0;                                    // clear TF3 overflow flag

    phase_acc += phase_add;                     // increment phase accumulator

    // set the value of <temp1> to the next output of DAC1 at full-scale
    // amplitude; the  rails are +32767, -32768
    switch (output_waveform)
    {
        case SINE:
            // read the table value
            if ( phase_acc < PHASE_HALF )
            {
                temp1 = *(table_ptr + (phase_acc >> 8));
            //  temp1 = SINE_TABLE[phase_acc/256];  // [i=0..127]
            } else {
                temp1 = - *(table_ptr + ((phase_acc - PHASE_HALF) >> 8));
                                                    // [i=128..255] -> [256-i]
            //  temp1 = -SINE_TABLE[(phase_acc - PHASE_HALF)/256];  // [i=0..127]
            }
            break;

        case SQUARE:
            // if in the first half-period, then high
            if ( phase_acc < PHASE_HALF )
            {
                temp1 = PHASE_HALF;
            } else {
                temp1 = -PHASE_HALF;
            }
            break;

        case TRIANGLE:
            // in first half-period, then  y = mx + b
            if ( phase_acc < PHASE_HALF )
            {
                temp1 = (phase_acc << 1) - PHASE_HALF;
            // else, in the second half of period
            } else {
                temp1 = -(phase_acc << 1) + PHASE_HALF;
            }
            break;

        case SAW:
            temp1 = phase_acc - PHASE_HALF;
        break;

        case OFF:
        default:
            temp1 = -PHASE_HALF;
            break;
    }

    // Adjust the Gain
    temp2 = (int32_t)amplitude * ( (int32_t)temp1 + (int32_t)(offset - PHASE_HALF) );
    temp1 = (int16_t)(temp2 >> 16);   // take only the two most significnat bytes
    
    SFRPAGE = DAC0_PAGE;                // set SFR page
    DAC0 = 0x8000 ^ temp1;              // Add a DC bias to make the rails 0 to 65535
    // DAC0 = 0x8000 + temp1;   // ^ trick works on the most significant digit only
    //                          // also prevents undefined roll over of signed short int16_t
    // Note: the XOR with 0x8000 translates the bipolar quantity into a unipolar quantity.
    // This is an interrupt, the original SFR page will be restored upon return from it
}
