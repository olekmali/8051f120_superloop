#ifndef _BU_UART
#define _BU_UART
#include <stdint.h>

void UART_Init (uint32_t sysclk, uint32_t baudrate);
void UART_Quit (void);

uint8_t ready_getchar (void);
uint8_t ready_putchar (void);

    #ifdef SDCC
int getchar (void);
int putchar (int c);
    #endif

#endif
