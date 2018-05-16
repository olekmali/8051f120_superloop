// Copyright (C) 2010 Haritha Kallamadi
#include <C8051F120.h>
#include "C8051F120_io.h"

#include "bu_init.h"
#include "bu_uart.h"
#include "bu_com.h"
#include "periodpca.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define BAUDRATE         9600U          // Baud rate of UART in bps
#define BUFLEN             16

void main(void)
{
    __xdata char buffer[BUFLEN+1];

    // Disable watchdog timer
    WDTCN = 0xde;
    WDTCN = 0xad;

    PORT_Init ();
    SYSCLK_Init();
    UART_Init(SYSCLK, BAUDRATE);

    // Using PCA0 to calculate the time period/frequency of encoder pulses using +VE edge triggering mode
    PCA0_Init();
    EA = 1;

    while (1) {
        uint16_t value = getRecentPeriod() * (SYSCLK/12/1000);
        sprintf(buffer,"%u", value );
        UART_puts("\nPeriod = ");
        UART_puts(buffer);
        UART_puts(" microseconds. Press ENTER >");
        UART_gets(buffer, sizeof(buffer));
    }
}
