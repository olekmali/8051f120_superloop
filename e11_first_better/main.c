#include <stdint.h>



// *** this below goes to a different library(ies) - .H and .C files ***
#include <C8051F120.h>

void DisableWatchdog() 
{
    WDTCN     = 0xDE;   // Watchdog timer must be either disabled or preferably reset 
    WDTCN     = 0xAD;   // every not more than certain number of milliseconds to prevent the board reboot
}

void ConfigureIO()
{
    SFRPAGE   = CONFIG_PAGE;    // This SFR setting may be important for configuring and accessing ports >=4
    P1MDOUT   = 0x40;   // 0b 0100 0000             - enable a bit for push-pull, reminder bits are still open drain
    P3MDOUT   = 0x00;   // which is also the default state after reset   - all output drivers are open drain
    P3        = 0xFF;   // which is also the default default state after reset   
                        // - high output produces high impedance of the outputs given open drain set by P3MDOUT
    XBR2      = 0x40;   // 0b01000000 - enable weak pull up resistors on IO so that no input is "floating" open
}

void setLED(uint8_t val)
{
    // the main development board LED
    if (val!=0) P1_6 = 1;
    else        P1_6 = 0;
}

uint8_t getLED()
{
    // the main development board LED
    return(P1_6);
}

uint8_t getButton()
{
    // the main development board push button
    return(P3_7);
}
// *** this above goes to a different library(ies) - .H and .C files ***



void main() {
    uint8_t state;

    DisableWatchdog();
    ConfigureIO();

    state = getButton();   
    setLED(1);      

    while(1)
    {
        if ( state!=getButton() )
        {
            if (state) 
            {
                setLED( ! getLED() );
            }
            state=getButton();
        }
    }
}
