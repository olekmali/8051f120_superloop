/*---------------------------------------------------------------------------
;
;
;
;
; FILE NAME: c8051F120_io.h
; TARGET MCUs: C8051F120, F121, F122, F123, F124, F125, F126, F127
; DESCRIPTION: Switch and LED definitions for SiLabs 8051F120 Dev Kit board and AB4 extension board
;
; REVISION 1.2
;
;---------------------------------------------------------------------------*/
#ifndef C8051F120_IO_H
#define C8051F120_IO_H

sbit LED        = P1^6;             // LED='1' means ON
                                    // To be used requires setting it to push--pull: P1MDOUT |= 0x40

sbit SW2        = P3^7;             // SW2='0' means switch pressed
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


sbit AB4_SW1    = P4^1;             // SW1 on AB4 Board
sbit AB4_SW2    = P4^2;             // SW2 on AB4 Board
                                    // Require not setting these bits to 1 in P4MDOUT
sbit AB4_LED1   = P4^3;             // Green AB4 Board LED
sbit AB4_LED2   = P4^4;             // Red AB4 Board LED
                                    // To be used require: P4MDOUT |= 0x18

#endif
