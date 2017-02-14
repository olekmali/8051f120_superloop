#include "C8051F120.h"

void main() {
    __bit state;

    WDTCN     = 0xDE;
    WDTCN     = 0xAD;

    SFRPAGE   = CONFIG_PAGE;    // This SFR setting may be important for configuring and accessing ports >=4

    P1MDOUT   = 0x40;   // 0b 0100 0000             - enable a bit for push-pull, reminder bits are still open drain
//  P3MDOUT   = 0x00; - default state after reset   - all bits are open drain
//  P3        = 0xFF; - default state after reset   - all outputs are high which is high impedance given open drain P3MDOUT

    XBR2      = 0x40;   // 0b 0100 0000             - enable pull up resistors on IO crossbar

    state     = P3_7;
    P1_6      = 1;      // LED

    while(1)
    {
        if (state!=P3_7)
        {
            if (state) 
            {
                P1_6 = ! P1_6;
            }
            state=P3_7;
        }
    }
}
