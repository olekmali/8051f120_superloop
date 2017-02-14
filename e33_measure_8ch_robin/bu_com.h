#ifndef _BU_COM
#define _BU_COM
#include <stdint.h>

void UART_puts (const char* buffer);
void UART_gets (      char* buffer, uint16_t len);
void UART_gets_noecho(char* buffer, uint16_t len);

void UART_write(const char* buffer, uint16_t len);
void UART_read (      char* buffer, uint16_t len);

#endif
