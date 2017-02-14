#include "bu_init.h"

#include "C8051F120.h"                         // Device-specific SFR Definitions

//-----------------------------------------------------------------------------
// Initialization Routines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// PORT_Init
//-----------------------------------------------------------------------------
//
// Configure the SPI, Interrupts, Crossbar and GPIO ports
//
void PORT_Init(void)
{
    char SFRPAGE_SAVE = SFRPAGE;    // Save Current SFR page

    SFRPAGE = CONFIG_PAGE;          // set SFR page
    P0MDOUT |= 0x01;                // Set TX1 pin to push-pull
    P1MDOUT |= 0x40;                // Set P1.6(TB_LED) to push-pull
//  P2MDOUT |= 0x00;
//  P3MDOUT |= 0x00;

    // all pins used by the external memory interface are in push-pull mode
    // including /WR (P4.7) and /RD (P4.6) but the reset (P4.5) is open-drain
    // P4MDOUT =  0xC0;
    // P4 = 0xC0;                   // /WR, /RD, are high, RESET is low

    // You may want to set P4.3 (AB4_LED1) and P4.4 (AB4_LED2) to push-pull
    // You may want to set P4.1 (AB4_SW1)  and P4.2 (AM4_SW2)  to open-drain
    P4MDOUT =  0xD8;
    // You may want to prevent permanent on on AB4 switches by not pulling them down
    P4 = 0xC6;                      // /WR, /RD, SW1, SW2 are high, RESET is low, 


    P5MDOUT =  0xFF;                // P5, P6 contain the address lines
    P6MDOUT =  0xFF;                // P5, P6 contain the address lines
    P7MDOUT =  0xFF;                // P7 contains the data lines
    P5 = 0xFF;                      // P5, P6 contain the address lines
    P6 = 0xFF;                      // P5, P6 contain the address lines
    P7 = 0xFF;                      // P7 contains the data lines

    TCON &= ~0x01;                  // Make /INT0 level triggered

    XBR0 = 0x80;                    // Enable CP0, Close PCA0 I/O, Close UART0
    XBR1 = 0x04;                    // Enable INT0 input pin, this puts /INT0 on P0.3.
    XBR2 = 0x44;                    // Enable crossbar and weak pull-up, Enable UART1

    SFRPAGE = SFRPAGE_SAVE;         // Restore SFR page
}

//-----------------------------------------------------------------------------
// SYSCLK_Init
//-----------------------------------------------------------------------------
//
// This routine initializes the system clock.
//
void SYSCLK_Init(void)
{
    volatile int i;                 // software delay variable
    char SFRPAGE_SAVE = SFRPAGE;    // Save Current SFR page
    SFRPAGE = CONFIG_PAGE;          // set SFR page

    OSCICN = 0x83;                  // set internal oscillator to run
                                    // at its maximum frequency

    CLKSEL = 0x00;                  // Select the internal osc. as
                                    // the SYSCLK source

    //Turn on the PLL and increase the system clock by a factor of M/N
    PLL0CN  = 0x01;                 // Enable Power to PLL and set internal osc. as PLL source
    PLL0DIV = 0x01;                 // Set Pre-divide value to N (N = 1)
    PLL0MUL = 0x03;                 // Multiply SYSCLK by M (M=3)
    PLL0FLT = 0x01;                 // Set the PLL filter register for
                                    // 0x*1 a reference clock from 19 to 30 MHz
                                    // 0x0* an output clock from 65 to 100 MHz

    for (i=0; i < 15; i++) ;        // Wait at least 5us at 24Mhz
    PLL0CN  |= 0x02;                // Enable the PLL
    while(!(PLL0CN & 0x10));        // Wait until PLL frequency is locked
    CLKSEL  = 0x02;                 // Select PLL as SYSCLK source

    SFRPAGE = LEGACY_PAGE;
    FLSCL   = 0x30;                 // Set FLASH read time for 100 MHz clk

    SFRPAGE = SFRPAGE_SAVE;         // Restore SFR page
}
