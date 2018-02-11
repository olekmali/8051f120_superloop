#include "bu_uart.h"
#include "C8051F120.h"              // Device-specific SFR Definitions

#ifdef SDCC
char * gets_safe(char *s, short n);
#endif

//-----------------------------------------------------------------------------
// UART_Init
//-----------------------------------------------------------------------------
//
// Configure the UART1 using Timer1, for <baudrate> and 8-N-1.
//
void UART_Init (unsigned long sysclk, unsigned long baudrate)
{
    char SFRPAGE_SAVE = SFRPAGE;    // Save the current SFR page
    const unsigned long int sysclkoverbaud = sysclk/baudrate;

    SFRPAGE = UART_PAGE;
    SCON1   = 0x10;                 // SCON1: mode 0, 8-bit UART, enable RX

    SFRPAGE = TIMER01_PAGE;
    TMOD   &= ~0xF0;
    TMOD   |=  0x20;                // TMOD: timer 1, mode 2, 8-bit reload


    // Set Timer 1 timebase.
    // Note: Since Timer 0 is used by the TCP/IP Library and forces the
    // shared T0/T1 prescaler to sysclk/48, Timer 1 may only be clocked
    // from sysclk or sysclk/48

    // If reload value is less than 8-bits, select sysclk
    // as Timer 1 baud rate generator
    if (sysclkoverbaud>>9 < 1)
    {
        TH1 = -(sysclkoverbaud>>1);
        CKCON |= 0x10;              // T1M = 1; SCA1:0 = xx

        // Otherwise, select sysclk/48 prescaler.
    }
    else
    {
        // Adjust for truncation in special case
        // Note: Additional cases may be required if the system clock is changed.
        if ((baudrate == 115200) && (sysclk == 98000000)) {
            TH1 = -((sysclkoverbaud/2/48)+1);
        } else {
            TH1 = -(sysclkoverbaud/2/48);
        }

        CKCON &= ~0x13;             // Clear all T1 related bits
        CKCON |=  0x02;             // T1M = 0; SCA1:0 = 10
    }

    TL1 = TH1;                      // initialize Timer1
    TR1 = 1;                        // start Timer1

    SFRPAGE = UART_PAGE;
    TI1 = 1;                        // Indicate TX1 ready

    SFRPAGE = SFRPAGE_SAVE;         // Restore the original SFR page
}


void UART_Quit (void)
{
    char SFRPAGE_SAVE = SFRPAGE;    // Save the current SFR page
    
    // Disable Timer1
    SFRPAGE = TIMER01_PAGE;
    TR1 = 0;                        // Stop Timer1
    TMOD = 0x00;                    // Restore the TMOD register to its reset value
    CKCON = 0x00;                   // Restore the CKCON register to its reset value
    
    // Disable UART1
    SFRPAGE = UART_PAGE;
    SCON1 = 0x00;                   // Disable UART1
    
    SFRPAGE = SFRPAGE_SAVE;         // Restore the original SFR page
}

unsigned char ready_getchar (void) {
    char SFRPAGE_SAVE = SFRPAGE;    // Save the current SFR page
    unsigned char c;
    SFRPAGE = UART_PAGE;
    c = RI1;
    SFRPAGE = SFRPAGE_SAVE;         // Restore the original SFR page
    return (c);
}

unsigned char ready_putchar (void) {
    char SFRPAGE_SAVE = SFRPAGE;    // Save the current SFR page
    unsigned char c;
    SFRPAGE = UART_PAGE;
    c = TI1;
    SFRPAGE = SFRPAGE_SAVE;         // Restore the original SFR page
    return (c);
}


#ifdef SDCC

char getchar ()  {
    char SFRPAGE_SAVE = SFRPAGE;    // Save the current SFR page
    char c;
    SFRPAGE = UART_PAGE;
    while (!RI1);
    c = SBUF1;
    RI1 = 0;
    SFRPAGE = SFRPAGE_SAVE;         // Restore the original SFR page
    return (c);
}

void putchar (char c)  {
    char SFRPAGE_SAVE = SFRPAGE;    // Save the current SFR page
    SFRPAGE = UART_PAGE;
    while (!TI1);
    TI1 = 0;
    SBUF1 = c;
    SFRPAGE = SFRPAGE_SAVE;         // Restore the original SFR page
}

#endif
