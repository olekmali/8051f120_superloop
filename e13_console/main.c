// Copyright (C) 2008-2017 Aleksander Malinowski

#include "C8051F120.h"                  // Device-specific SFR Definitions
#include "C8051F120_io.h"               // Device-specific SFR Definitions

#include "bu_init.h"
#include "bu_uart.h"
#include "bu_com.h"

#include <stdint.h>
#include <stdio.h>

#define BAUDRATE     9600U             // Baud rate of UART in bps

void main(void)
{
    uint8_t state;
    uint16_t  counter = 0;
    static __xdata char buffer[4];
    static __xdata char buf_test[20];

    // Disable watchdog timer
    WDTCN = 0xde;
    WDTCN = 0xad;

    // Initialize the MCU
    PORT_Init();
    SYSCLK_Init();
    UART_Init(SYSCLK, BAUDRATE);

    state = SW2;
    LED = 1;
    UART_puts("System Ready!\n\n");

    UART_puts("Testing bidirectional serial I/O...\n");
    UART_puts("What's your name? ");
    UART_gets(buf_test, sizeof(buf_test));
    UART_puts("You entered name: ");
    UART_puts(buf_test);
    UART_puts("\n");

    UART_puts("Testing board buttons...\n");
    while(1)
    {
        if (state != SW2)
        {
            if (state==1)
            {
                UART_puts("Switch depressed\n");
                LED = ! LED;

                if (LED==1) UART_puts("LED is now on\n");
                else UART_puts("LED is now off\n");

                counter++;
                UART_puts("The switch was pressed ");
                sprintf(buffer, "%d", counter);
                UART_puts(buffer);
                UART_puts(" times.\n");
            } else {
                UART_puts("Switch released\n");
            }
            state = SW2;
        }
    }
}
