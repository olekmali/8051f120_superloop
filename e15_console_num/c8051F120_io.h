/*---------------------------------------------------------------------------
;
;
;
;
; FILE NAME: C8051F120.H
; TARGET MCUs: C8051F120, F121, F122, F123, F124, F125, F126, F127
; DESCRIPTION: Switch and LED definitions
;
; REVISION 1.2
;
;---------------------------------------------------------------------------*/
#ifndef C8051F120_IO_H
#define C8051F120_IO_H

__sbit __at (0x96) LED;             // LED='1' means ON
                                    // To be used requires setting it to push--pull: P1MDOUT |= 0x40

__sbit __at (0xB7) SW2;             // SW2='0' means switch pressed
                                    // Requires open-drain: i.e. not setting that bit to 1 in P3MDOUT


// Note: AB4 switches are accessible on CONFIG_PAGE SFR Page only
// Pins 1 through 4 of port P4 must be configured
// Leave alone Pins 0, and 5-7 of P4. Pin 0 can be reused in the future but
// Pins 5-7 of P4 are used by Ethernet controller and must not be changed
//
//  SFRPAGE = CONFIG_PAGE;      // set SFR page to page 0x0F from which switch P4 and AB4 LEDs and Switches are controlled
//  P4 = P4 | 0x06;             // Make sure that bits P4.1 and P4.2 are set high to prevent permament switch ON
//  P4MDOUT = P4MDOUT & 0xF9;   // Make sure that P4.1 and P4.2 are Open Collector for reading switches
//  P4MDOUT = P4MDOUT | 0x18;   // Make sure that P4.3 and P4.4 are Push/Pull for driving LEDs
//  Set SFR Page again before using AB4_* if it is changed elesewhere in the program


__sbit __at (0xC9) AB4_SW1;         // SW1 on AB4 Board
__sbit __at (0xCA) AB4_SW2;         // SW2 on AB4 Board
                                    // Require not setting these bits to 1 in P4MDOUT
__sbit __at (0xCB) AB4_LED1;        // Green AB4 Board LED
__sbit __at (0xCC) AB4_LED2;        // Red AB4 Board LED
                                    // To be used require: P4MDOUT |= 0x18

#endif
