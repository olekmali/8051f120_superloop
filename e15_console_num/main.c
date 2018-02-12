#include "C8051F120.h"                  // Device-specific SFR Definitions
#include "C8051F120_io.h"               // Device-specific SFR Definitions

#include "bu_init.h"
#include "bu_uart.h"
#include "bu_com.h"

#include <stdint.h>
#include <stdio.h>                      // sprintf
#include <stdlib.h>                     // atoi, atol, atof

#define BAUDRATE     9600U              // Baud rate of UART in bps

void main(void)
{
    uint8_t state;
    uint16_t  counter = 0;
    __xdata char buffer[4];

    // Disable watchdog timer
    WDTCN = 0xde;
    WDTCN = 0xad;

    // Initialize the MCU
    PORT_Init();
    SYSCLK_Init();
    UART_Init(SYSCLK, BAUDRATE);

    LED = 0;
    UART_puts("System Ready!\n\n");

    UART_puts("Demonstrating conversion between ASCII and NUMBERS\n");
    // Note: this particular conversion could be done much simpler
    //       as it only uses one digit number that is one character 
    do {
        UART_puts("Please enter the initial state for the LED light (0 or 1) ");
        UART_gets(buffer, sizeof(buffer));
        state = atoi(buffer); 
    } while (state!=0 && state!=1);
    LED = state;

/*
    // Note: this particular conversion works only for one character numbers 
    do {
        UART_puts("Please enter the initial state for the LED light (0 or 1) ");
        UART_gets(buffer, sizeof(buffer));
    } while (buffer[0]!='0' && buffer[0]!='1');
    LED = (buffer[0]=='1'); // boolean: false==0, true==1
    // Note for numbers from 0 to 9 use:
    //      } while (buffer[0]<'0' || buffer[0]>'9');
    //      variable = buffer[0]-'0'; 
*/


    UART_puts("Testing board buttons...\n");
    state = SW2;
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
