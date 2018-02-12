// Copyright (C) 2007 Dexter Travis
#include "C8051F120.h"                  // Device-specific SFR Definitions
#include "C8051F120_io.h"

#include "bu_init.h"
#include "bu_uart.h"
#include "bu_com.h"
#include "pwmpca.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define BAUDRATE     9600U              // Baud rate of UART in bps
#define BUFLEN         20

void main(void)
{
    char buffer[BUFLEN+1];
    uint8_t channel = 0;
    uint8_t value;

    // Disable watchdog timer
    WDTCN = 0xde;
    WDTCN = 0xad;

    PORT_Init ();
    SYSCLK_Init();
    UART_Init(SYSCLK, BAUDRATE);

    PCA0_Init();
    EA = 1;

    while (1) {

        UART_puts("\nPWM 6-channel generator on P3.0 to P3.5\n\nMain Menu:\n\nR          channel Reset to defaults\nC <chann>  channel set    0..5\n0          channel OFF\n1          channel ON\nD <number> channel DUTY      0..100%\n\n> ");
        UART_gets(buffer, sizeof(buffer));

        switch(buffer[0])
        {
            case '0' : PCA0_SetOn(channel, OFF);
                break;
            case '1' : PCA0_SetOn(channel, ON);
                break;
            case 'c' :
            case 'C' :
                value = (uint8_t)atoi(&buffer[1]);
                if (value<6) channel = value;
                else UART_puts("Channel out of range\n");
                break;
            case 'd' :
            case 'D' :
                value = atoi(&buffer[1]);
                if (value<100) PCA0_SetDuty(channel, value);
                else UART_puts("PWM duty out of range\n");
                break;
            case 'r' :
            case 'R' :
                PCA0_SetDuty(channel, 50);
                PCA0_SetOn(channel, ON);
                break;
            default:
                UART_puts("Invalid command\n");
        }

    }
}
