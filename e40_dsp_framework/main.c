// Copyright (C) 2008-2017 Aleksander Malinowski

#include "C8051F120.h"                  // Device-specific SFR Definitions
#include "adc0dsp.h"

#include "bu_init.h"
#include "bu_wait.h"
#include "bu_uart.h"
#include "bu_com.h"

#include <stdint.h>
#include <stdio.h>

#define BAUDRATE     9600U              // Baud rate of UART in bps
#define SAMPLE_RATE  8000L              // Sample frequency in Hz

void main(void)
{
    // Disable watchdog timer
    WDTCN = 0xde;
    WDTCN = 0xad;

    PORT_Init ();
    SYSCLK_Init();
    UART_Init(SYSCLK, BAUDRATE);

    ADC0_DACs_Timer3_Init(SYSCLK, SAMPLE_RATE);
    // Needs global interrupts enabled to work

    EA = 1;                             // Enable global interrupts

    while (1)
    {
        UART_puts("Still alive\n");
        wait_ms(SYSCLK, 1000);                  // wait 1000 milliseconds
    }

}
