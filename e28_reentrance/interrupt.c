#include "interrupt.h"

#include "C8051F120.h"                  // SFR declarations
#include "C8051F120_io.h"               // SFR declarations


//------------------------------------------------------------------------------------
// Timer4_Init
//------------------------------------------------------------------------------------
//
// Configure Timer4 to auto-reload and generate an interrupt at interval
// specified by <counts> using SYSCLK/12 as its time base.
//
void Timer4_Init (uint32_t sysclock, uint32_t rate)
{
    uint8_t SFRPAGE_SAVE = SFRPAGE;        // Save Current SFR page
    int32_t counts = sysclock/(12*rate);   // Note that Timer4 is connected to SYSCLK/12

    SFRPAGE = CONFIG_PAGE;              // set SFR page
    P3MDOUT |= 0x3F;                    // Set P3.0 through P3.5 to push-pull

    SFRPAGE = TMR4_PAGE;                // set SFR page
    TMR4CN  = 0x00;                     // Stop Timer4; Clear TF4;
    TMR4CF  = 0x00;                     // use SYSCLK/12 as timebase
//  TMR4CF  = 0x08;                     // use SYSCLK as timebase
    RCAP4   = 65536 -(uint16_t)counts;  // Init reload values
    // or   = -(uint16_t)counts; -- see the in class comment
    TMR4    = RCAP4;                    // set starting value
    EIE2   |= 0x04;                     // enable Timer4 interrupts - bit 00000100 or ET4 = 1;
    TMR4CN |= 0x04;                     // start Timer4

    SFRPAGE = SFRPAGE_SAVE;             // Restore SFR page
}

//------------------------------------------------------------------------------------
// Interrupt Service Routines
//------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Timer4_ISR -- Some number Crunching
//-----------------------------------------------------------------------------
//
/* The generator used to keep this interrupt busy is a so-called 
 * Lehmer random number generator' which returns a pseudo-random number 
 * uniformly distributed between 0 and MODULUS. The period is (MODULUS-1).
 * For more details see:
 *       "Random Number Generators: Good Ones Are Hard To Find"
 *                   Steve Park and Keith Miller
 *              Communications of the ACM, October 1988
 */
#define MODULUS    2147483647L /* DON'T CHANGE THIS VALUE                  */
#define MULTIPLIER 48271L      /* DON'T CHANGE THIS VALUE                  */
#define DEFAULT    123456789L  /* initial seed, use 0 < DEFAULT < MODULUS  */
//
void Timer4_ISR (void) __interrupt 16
{
    static int32_t seed = DEFAULT;
    const int32_t Q = MODULUS / MULTIPLIER;
    const int32_t R = MODULUS % MULTIPLIER;
    
    SFRPAGE = TMR4_PAGE;            // set SFR page
    TMR4CN &= ~0x80;                // clear T4 overflow flag

    seed = MULTIPLIER * (seed % Q) - R * (seed / Q);
    if (seed <= 0) seed = seed + MODULUS;
    // 0<= random number < MODULS
    // for a desired range 0<= random number <range use: seed*range/MODULUS

    // This is an interrupt, the original SFR page will be restored upon return from it
}
