// Copyright (C) 2008-2017 Aleksander Malinowski

#include "C8051F120.h"                  // Device-specific SFR Definitions
#include "C8051F120_io.h"

#include "bu_init.h"
#include "bu_uart.h"
#include "bu_com.h"
#include "pwmint.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define BAUDRATE         9600U          // Baud rate of UART in bps
#define BUFLEN             20           // Maximum user buffer size
#define PWM_FREQUENCY    1000           // Interrupt frequency in Hz - high to accommodate high range of PWM frequencies
#define PWM_RESOLUTION    100 

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
    UART_Init(SYSCLK, BAUDRATE);

    // using Timer4 as update scheduler initialize T4 to update DAC1 after (SYSCLK cycles)/sample have passed.
    Timer4_PWM_Init (SYSCLK, PWM_FREQUENCY, PWM_RESOLUTION);
    EA = 1;

    while (1) {

        UART_puts("\nPWM 6-channel generator on P3.0 to P3.5\n\nMain Menu:\nF <number>           FREQUENCY in Hz\nR                    RESET DEFAULTS\n\nC <chann>  channel to set    0..5\n0          channel OFF\n1          channel ON\nD <number> channel DUTY      0..100%\n\n> ");
        UART_gets(buffer, sizeof(buffer));

        switch(buffer[0])
        {
            case '0' : Timer4_PWM_SetOn(channel, OFF);
                break;
            case '1' : Timer4_PWM_SetOn(channel, ON);
                break;
            case 'c' :
            case 'C' :
                value = atoi(&buffer[1]);
                if (NUM_PWM_CHANNELS>value) channel = value;
                else UART_puts("Channel out of range\n");
                break;
            case 'd' :
            case 'D' :
                value = atoi(&buffer[1]);
                if (PWM_RESOLUTION>=value) Timer4_PWM_SetDuty(channel, value);
                else UART_puts("PWM duty out of range\n");
                break;
            case 'f' :
            case 'F' :
                UART_puts("Frequency is constant at 1000Hz\n");
                break;
            case 'r' :
            case 'R' :
                Timer4_PWM_SetDuty(channel, 50);
                Timer4_PWM_SetOn(channel, ON);
                break;
            default:
                UART_puts("Invalid command\n");
        }

    }

}
