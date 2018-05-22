#include "bu_com.h"
#include "bu_uart.h"
#include <stdio.h>                  // Add support for printf, putchar, getchar, etc.

#ifndef SDCC
#include <C8051F120.h>              // Device-specific SFR Definitions
#endif

#ifdef SDCC
char * gets_safe(char *s, uint16_t n);
#endif

void UART_puts(const char* buffer)
{
#ifndef SDCC
    uint8_t SFRPAGE_SAVE = SFRPAGE;    // Save the current SFR page
    SFRPAGE = UART1_PAGE;
#endif
    while (*buffer)
    {
        if ( '\n' == *buffer ) putchar(0x0d);   // add linefeed for ASCII transmission
        putchar(*buffer++);
    }
#ifndef SDCC
    SFRPAGE = SFRPAGE_SAVE;         // Restore the original SFR page
#endif
}

void UART_gets(char* buffer, uint16_t len)
{
#ifndef SDCC
    uint8_t SFRPAGE_SAVE = SFRPAGE; // Save the current SFR page
    SFRPAGE = UART1_PAGE;
    gets(buffer,len);
    SFRPAGE = SFRPAGE_SAVE;         // Restore the original SFR page
#else
    gets_safe(buffer, len);         // !! SDCC originally has no max buffer parameter !!
#endif
}

void UART_gets_noecho(char* buffer, uint16_t len)
{
    uint16_t count=0;
#ifndef SDCC
    uint8_t SFRPAGE_SAVE = SFRPAGE;    // Save the current SFR page
    SFRPAGE = UART1_PAGE;
#endif

    len--;
    while (1) {
        char c=getchar();
        if ( '\n'==c || '\r'==c )
        {
            *buffer=0;
            return;
        } else {
            if (count<len) {
                (*buffer++)=c;
                count++;
            }
        }
    }


#ifndef SDCC
    SFRPAGE = SFRPAGE_SAVE;         // Restore the original SFR page
#endif
}

void UART_write(const char* buffer, uint16_t len)
{
#ifndef SDCC
    uint8_t SFRPAGE_SAVE = SFRPAGE;    // Save the current SFR page
    SFRPAGE = UART1_PAGE;
#endif
    while(len>0)
    {
        putchar(*buffer);
        buffer++;
        --len;
    }
#ifndef SDCC
    SFRPAGE = SFRPAGE_SAVE;         // Restore the original SFR page
#endif
}

void UART_read (char* buffer, uint16_t len)
{
#ifndef SDCC
    uint8_t SFRPAGE_SAVE = SFRPAGE; // Save the current SFR page
    SFRPAGE = UART1_PAGE;
#endif
    while(len>0)
    {
        *buffer = getchar();
        buffer++;
        --len;
    }
#ifndef SDCC
    SFRPAGE = SFRPAGE_SAVE;         // Restore the original SFR page
#endif
}



#ifdef SDCC
char * gets_safe(char *s, uint16_t n) {
    char c;
    uint16_t count=0;
    n--;

    while (1) {
        c=getchar();
        switch(c) {
        case '\b': // backspace
            if (count) {
                putchar('\b');
                putchar(' ');
                putchar('\b');
                s--;
                count--;
            }
            break;
        case '\n':
        case '\r': // CR or LF
            putchar('\r');
            putchar('\n');
            *s=0;
            return s;
        default:
            if (count<n) {
                *s++=c;
                count++;
                putchar(c);
            } else {
                putchar('\a');
            }
            break;
        }
    }
}

#endif
