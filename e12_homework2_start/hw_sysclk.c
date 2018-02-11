//* ECE471/571 Homework 2 File

// Name: ___insert your name here___

#include "C8051F120.h"      // Device-specific SFR Definitions
#include "hw_sysclk.h"      // Header file with function prototype and definitions

#include <stdint.h>

static uint8_t _sysclk_mode = SYSCLK_no_change;



uint8_t get_sysclk_mode()
{
    return(_sysclk_mode);
}


void reinit_sysclk(uint8_t mode)
{
     // Note: modify the outline below as necessary
    volatile int16_t i;                 // software delay variable
    uint8_t SFRPAGE_SAVE = SFRPAGE;    // Save the current SFR page
    __bit EA_SAVE     = EA;         // Preserve Current Interrupt Status
    
    if ( SYSCLK_no_change == mode) return;

    SFRPAGE = CONFIG_PAGE;          // set the SFR page to allow access to the necessary SFRs
    EA = 0;                         // disable interrupts


    _sysclk_mode = mode;    // remember the mode

    if (SYSCLK_INT_98000KHz>=mode)
    { // INTERNAL oscillator MODES:

        // if internal oscillator disabled
            // power on internal internal oscillator
            // wait at least 5uS (or check how int32_t one should wait)

        if ( SYSCLK_INT_24500KHz>=mode )
        {
            // configure FLASH timing
            switch (mode)
            {
            case SYSCLK_INT_3062KHz:
                // set divide by 8 - set in OSCICN
                break;
            case SYSCLK_INT_6125KHz:
                // set divide by 4 - set in OSCICN
                break;
            case SYSCLK_INT_12250KHz:
                // set divide by 2 - set in OSCICN
                break;
            case SYSCLK_INT_24500KHz:
                // set divide by 1 - set in OSCICN
                break;
            }
            // switch sysclk to internal oscillator - set in CLKSEL
            // disable PLL to conserve energy
        } else {
            // if PLL selected in CLKSEL
                // switch sysclk to internal oscillator - set in CLKSEL
            // configure and enable PLL - internal oscillator as input
            // configure FLASH timing
            switch (mode)
            {
            case SYSCLK_INT_49000KHz:
                // set PLL0DIV, PLL0MUL, PLL0FLT
                break;
            case SYSCLK_INT_73500KHz:
                // set PLL0DIV, PLL0MUL, PLL0FLT
                break;
            case SYSCLK_INT_98000KHz:
                // set PLL0DIV, PLL0MUL, PLL0FLT
                break;
            }
            // wait at least 5us (iddling loop)
            // wait unitl PLL is locked
            // switch sysclk to PLL - set in CLKSEL
        }
        // disable external oscillator
    } else {
        // EXTERNAL oscillator MODES:

        // if external oscillator disabled
            // enable external oscillator
            // wait at least 1ms

        if ( SYSCLK_EXT_22118KHz==mode ) {
            // switch sysclk to external oscillator - set in CLKSEL
            // configure FLASH timing
            // disable PLL to conserve energy
        } else {
            // if PLL selected in CLKSEL
                // switch sysclk to external oscillator - set in CLKSEL
            // configure and enable PLL - external oscillator as input
            // configure FLASH timing
            switch (mode)
            {
            case SYSCLK_EXT_44237KHz:
                // set PLL0DIV, PLL0MUL, PLL0FLT
                break;
            case SYSCLK_EXT_66355KHz:
                // set PLL0DIV, PLL0MUL, PLL0FLT
                break;
            case SYSCLK_EXT_88474KHz:
                // set PLL0DIV, PLL0MUL, PLL0FLT
                break;
            }
            // wait at least 5us (iddling loop)
            // wait unitl PLL is locked
            // switch sysclk to PLL - set in CLKSEL
        }
        // disable internal oscillator
    }

    SFRPAGE = SFRPAGE_SAVE;         // Restore the original SFR page
    EA = EA_SAVE;                   // restore interrupts
}
