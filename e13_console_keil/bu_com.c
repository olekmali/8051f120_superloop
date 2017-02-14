#include "bu_com.h"
#include "bu_uart.h"
#include <stdio.h>                  // Add support for printf, putchar, getchar, etc.

#ifndef SDCC
#include "C8051F120.h"              // Device-specific SFR Definitions
#endif

#ifdef SDCC
char * gets_safe(char *s, unsigned int n);
#endif

void UART_puts(const char* buffer)
{
#ifdef SDCC
    while (*buffer)
    {
        if ( '\n' == *buffer ) putchar(0x0d);   // add linefeed for ASCII transmission
        putchar(*buffer++);
    }
#else
    char SFRPAGE_SAVE = SFRPAGE;    // Save Current SFR Page
    SFRPAGE = UART_PAGE;
    while (*buffer)
    {
        if ( '\n' == *buffer ) putchar(0x0d);   // add linefeed for ASCII transmission
        putchar(*buffer++);
    }
    SFRPAGE = SFRPAGE_SAVE;         // Restore SFR page
#endif
}

void UART_gets(char* buffer, unsigned int len)
{
#ifdef SDCC
    gets_safe(buffer,len);          // !! SDCC originally has no max buffer parameter !!
#else
    char SFRPAGE_SAVE = SFRPAGE;    // Save Current SFR Page
    SFRPAGE = UART_PAGE;
    gets(buffer,len);
    SFRPAGE = SFRPAGE_SAVE;         // Restore SFR page
#endif
}

void UART_gets_noecho(char* buffer, unsigned int len)
{
    unsigned int count=0;
#ifndef SDCC
    char SFRPAGE_SAVE = SFRPAGE;    // Save Current SFR Page
    SFRPAGE = UART_PAGE;
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
    SFRPAGE = SFRPAGE_SAVE;         // Restore SFR page
#endif
}

void UART_write(const char* buffer, unsigned int len)
{
#ifdef SDCC
    while(len>0)
    {
        putchar(*buffer++);
        --len;
    }
#else
    char SFRPAGE_SAVE = SFRPAGE;    // Save Current SFR Page
    SFRPAGE = UART_PAGE;
    while(len>0)
    {
        putchar(*buffer++);
        --len;
    }
    SFRPAGE = SFRPAGE_SAVE;         // Restore SFR page
#endif
}

void UART_read (char* buffer, unsigned int len)
{
#ifdef SDCC
    while(len>0)
    {
        (*buffer++)=getchar();
        --len;
    }
#else
    char SFRPAGE_SAVE = SFRPAGE;    // Save Current SFR Page
    SFRPAGE = UART_PAGE;
    while(len>0)
    {
        (*buffer++)=getchar();
        --len;
    }
    SFRPAGE = SFRPAGE_SAVE;         // Restore SFR page
#endif
}



#ifdef SDCC
char * gets_safe(char *s, unsigned int n) {
    char c;
    unsigned int count=0;
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
