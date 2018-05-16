#include "bu_reset.h"

#include <C8051F120.h>                  // Device-specific SFR Definitions

uint8_t getResetSource()
{
    uint8_t reason;
    uint8_t SFRPAGE_SAVE = SFRPAGE;     // Save the current SFR page
    SFRPAGE = LEGACY_PAGE;

    reason = RSTSRC;

    SFRPAGE= SFRPAGE_SAVE;              // Restore the original SFR page
    return(reason);
}

void doForceFullReset()
{
    SFRPAGE = LEGACY_PAGE;
    RSTSRC = 0x01;                      // Force RESET and low on RESET pin
    while(1)
        ;
}

void doForceSoftReset()
{
    SFRPAGE = LEGACY_PAGE;
    RSTSRC = 0x10;                      // Force internal RESET
    while(1)
        ;
}

