#ifndef _BU_UART
#define _BU_UART

void UART_Init (unsigned long sysclk, unsigned long baudrate);
void UART_Quit (void);

unsigned char ready_getchar (void);
unsigned char ready_putchar (void);

    #ifdef SDCC
char getchar (void);
void putchar (char c);
    #endif

#endif
