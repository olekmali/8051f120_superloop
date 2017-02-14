#ifndef _BU_COM
#define _BU_COM

void UART_puts (const char* buffer);
void UART_gets (      char* buffer, unsigned int len);
void UART_gets_noecho(char* buffer, unsigned int len);

void UART_write(const char* buffer, unsigned int len);
void UART_read (      char* buffer, unsigned int len);

#endif
