#include "C8051F120.h"                  // Device-specific SFR Definitions
#include "C8051F120_io.h"               // Device-specific SFR Definitions

#include "bu_init.h"
#include "bu_uart.h"
#include "bu_com.h"

#include <stdint.h>
#include <stdio.h>

#define BAUDRATE     9600U              // Baud rate of UART in bps

void main(void)
{
    static __xdata char buf_test[40];

    // Disable watchdog timer
    WDTCN = 0xde;
    WDTCN = 0xad;

    // Initialize the MCU
    PORT_Init();
    SYSCLK_Init();
    UART_Init(SYSCLK, BAUDRATE);

    LED = 1;

    while(1)
    {
        UART_puts("\nGoon: ");
        UART_gets(buf_test, sizeof(buf_test));
        UART_puts("\nRead: ");
        UART_puts(buf_test);
        LED = ! LED;

    }
}
