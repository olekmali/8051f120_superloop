// Copyright (C) 2008-2017 Aleksander Malinowski

#include "C8051F120.h"                  // Device-specific SFR Definitions
#include "C8051F120_io.h"

#include "bu_init.h"
#include "bu_uart.h"
#include "bu_com.h"
#include "pwmpca.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define BAUDRATE     9600               // Baud rate of UART in bps
#define BUFLEN         20

void main(void)
{
    char buffer[BUFLEN+1];
    uint8_t channel = 0;
    uint32_t value;

    // Disable watchdog timer
    WDTCN = 0xde;
    WDTCN = 0xad;

    PORT_Init ();
    SYSCLK_Init();
    UART_Init(SYSCLK, 9600);

    PCA0_PWM_Init(SYSCLK);
    EA = 1;

    while (1) {

        UART_puts("\nSQR Wave 6-channel generator on P3.0 to P3.5\n\nMain Menu:\n\nR          channel Reset to defaults\nC <chann>  channel set    0..5\n0          channel OFF\n1          channel ON\nF <number>         FREQUENCY in Hz\nO <number>         OFFSET in degrees\n\n> ");
        UART_gets(buffer, sizeof(buffer));

        switch(buffer[0])
        {
            case '0' : PCA0_PWM_SetOn(channel, OFF);
                break;
            case '1' : PCA0_PWM_SetOn(channel, ON);
                break;
            case 'c' :
            case 'C' :
                value = atoi(&buffer[1]);
                if (value<6) channel = value;
                else UART_puts("Channel out of range\n");
                break;
            case 'f' :
            case 'F' :
                value = atol(&buffer[1]);
                if (value>=100) PCA0_PWM_SetFrequency(channel, value);
                else UART_puts("PWM frequency too low\n");
                break;
            case 'o' :
            case 'O' :
                value = atoi(&buffer[1]);
                if (value<360) PCA0_PWM_SetOffset(channel, value);
                else UART_puts("PWM offset out of range\n");
                break;
            case 'r' :
            case 'R' :
                PCA0_PWM_SetFrequency(channel, 500);
                PCA0_PWM_SetOn(channel, ON);
                break;
            default:
                UART_puts("Invalid command\n");
        }

    }
}
