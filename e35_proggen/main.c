// Copyright (C) 2008-2017 Aleksander Malinowski

#include "C8051F120.h"                  // Device-specific SFR Definitions
#include "C8051F120_io.h"

#include "bu_init.h"
#include "bu_uart.h"
#include "bu_com.h"
#include "dac0int.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define BAUDRATE     9600U              // Baud rate of UART in bps
#define SAMPLE_RATE (100000L)           // Sample frequency in Hz
#define BUFLEN         20

void main(void)
{
    char buffer[BUFLEN+1];
    uint32_t value;

    // Disable watchdog timer
    WDTCN = 0xde;
    WDTCN = 0xad;

    PORT_Init ();
    SYSCLK_Init();
    UART_Init(SYSCLK, BAUDRATE);

    // using Timer4 as update scheduler initialize T4 to update DAC1 after (SYSCLK cycles)/sample have passed.
    DAC0_Timer3_Init (SYSCLK, SAMPLE_RATE);
    EA = 1;

    DAC0_Timer3_SetType(OFF);


    while (1) {

        UART_puts("\nMain Menu:\n0 OFF\n1 SQUARE\n2 SINE\n3 TRIANGLE\n4 SAW\n\nF <number> FREQUENCY in Hz\nA <number> AMPLITUDE 0..100%\nO <number> OFFSET    0..100%\nD defaults\n\n> ");
        UART_gets(buffer,sizeof(buffer));

        switch(buffer[0])
        {
            case '0' : DAC0_Timer3_SetType(OFF);        break;
            case '1' : DAC0_Timer3_SetType(SQUARE);     break;
            case '2' : DAC0_Timer3_SetType(SINE);       break;
            case '3' : DAC0_Timer3_SetType(TRIANGLE);   break;
            case '4' : DAC0_Timer3_SetType(SAW);        break;
            case 'f' :
            case 'F' :
                value = atol(&buffer[1]);
                DAC0_Timer3_SetFrequency(value);
                break;
            case 'a' :
            case 'A' :
                value = atol(&buffer[1]);
                DAC0_Timer3_SetAmplitude(value);
                break;
            case 'o' :
            case 'O' :
                value = atol(&buffer[1]);
                DAC0_Timer3_SetOffset(value);
                break;
            case 'd' :
            case 'D' :
                DAC0_Timer3_SetType(SINE);
                DAC0_Timer3_SetFrequency(1000);
                DAC0_Timer3_SetAmplitude(100);
                DAC0_Timer3_SetOffset(50);
                break;
            default:
                UART_puts("Invalid command\n");
        }
    }
}
