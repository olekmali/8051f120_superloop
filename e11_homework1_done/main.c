#include <C8051F120.h>


__sbit __at (0x96) LED1;            // LED='1' means ON
__sbit __at (0xB7) SW2;             // SW2='0' means switch pressed
__sbit __at (0xC9) AB4_SW1;         // SW1 on AB4 Board
__sbit __at (0xCA) AB4_SW2;         // SW2 on AB4 Board
__sbit __at (0xCB) AB4_LED1;        // Green AB4 Board LED
__sbit __at (0xCC) AB4_LED2;        // Red AB4 Board LED


void main() {
    __bit state1, state2, state3;

    WDTCN     = 0xDE;
    WDTCN     = 0xAD;

    SFRPAGE   = CONFIG_PAGE;
    P1MDOUT  |= 0x40;               // Set P1.6(TB_LED) to push-pull

    // You may want to set P4.3 (AB4_LED1) and P4.4 (AB4_LED2) to push-pull
    // You may want to set P4.1 (AB4_SW1)  and P4.2 (AM4_SW2)  to open-drain
    P4MDOUT =  0xD8;
    // You may want to prevent permanent on on AB4 switches by not pulling them down
    P4 = 0xC6;                      // /WR, /RD, SW1, SW2 are high, RESET is low, 


    XBR2        = 0x40; // Enable the crossbar and week pull up resistors

    state1      = SW2;
    state2      = AB4_SW1;
    state3      = AB4_SW2;
    LED1        = 1;
    AB4_LED1    = 1;
    AB4_LED2    = 1;



    while(1) {

        if (state1!=SW2) {
            if (state1) LED1 = ! LED1;
            state1=SW2;
        }

        if (state2!=AB4_SW1) {
            if (state2) AB4_LED1 = ! AB4_LED1;
            state2=AB4_SW1;
        }

        if (state3!=AB4_SW2) {
            if (state3) AB4_LED2 = ! AB4_LED2;
            state3=AB4_SW2;
        }

    }


}
