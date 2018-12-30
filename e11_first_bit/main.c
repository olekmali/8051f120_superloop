#include <C8051F120.h>

void main() {
    __bit state;        // excessive savings? On this processor family we can afford up to some 20 "optimized" bit variables
    // unsigned char state; // if there is no concern for RAM memory and/or there is no underlying support for single bit operations  
                            // then we could use the usual one byte variable to store just one bit

    WDTCN     = 0xDE;   // Watchdog timer must be either disabled or preferably reset 
    WDTCN     = 0xAD;   // every not more than certain number of milliseconds to prevent the board reboot

    SFRPAGE   = CONFIG_PAGE;    // This SFR setting may be important for configuring and accessing ports >=4

    P1MDOUT   = 0x40;   // 0b 0100 0000             - enable a bit for push-pull, reminder bits are still open drain
    P3MDOUT   = 0x00;   // which is also the default state after reset   - all output drivers are open drain
    P3        = 0xFF;   // which is also the default default state after reset   
                        // - high output produces high impedance of the outputs given open drain set by P3MDOUT

    XBR2      = 0x40;   // 0b01000000 - enable weak pull up resistors on IO so that no input is "floating" open

    state     = P3_7;   // the main development board push button -- this is a signle SFR bit access variable
    P1_6      = 1;      // the main development board LED         -- this is a signle SFR bit access variable

    while(1)
    {
        if ( state!=P3_7 )
        {
            if ( state!=0 ) 
            {
                P1_6 = ! P1_6;
            }
            state = P3_7;
        }
        // the button is debounced so there is no need for any delay before it is sampled again
    }
}
