// Copyright (C) 2010-2017 Aleksander Malinowski

#include "C8051F120.h"                  // Device-specific SFR Definitions
#include "C8051F120_io.h"               // Device-specific SFR Definitions

#include "bu_init.h"
#include "bu_uart.h"

#include <stdint.h>
#include <stdio.h>

#define BAUDRATE     9600               // Baud rate of UART in bps

void main(void)
{
    char ch='0';

    // Disable watchdog timer
    WDTCN = 0xde;
    WDTCN = 0xad;

    // Initialize the MCU
    PORT_Init();
    SYSCLK_Init();
    UART_Init(SYSCLK, 9600);

    LED = 1;

    while(1)
    {
        for(ch='0'; ch<='9'; ch++)
            putchar(ch);
        for(ch='A'; ch<='Z'; ch++)
            putchar(ch);
        for(ch='a'; ch<='z'; ch++)
            putchar(ch);

    }
}
