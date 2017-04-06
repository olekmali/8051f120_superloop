// Copyright (C) 2008-2017 Aleksander Malinowski

#include "C8051F120.h"                  // Device-specific SFR Definitions
#include "C8051F120_io.h"               // Device-specific SFR Definitions

#include "bu_init.h"
#include "bu_uart.h"
#include "bu_com.h"
#include "bu_flash.h"

#include <stdint.h>

#define BAUDRATE     9600U              // Baud rate of UART in bps

void main(void)
{
    uint8_t state;


    // Disable watchdog timer
    WDTCN = 0xde;
    WDTCN = 0xad;

    // Initialize the MCU
    PORT_Init();
    SYSCLK_Init();
    UART_Init(SYSCLK, BAUDRATE);
    FLASH_Init();
 
    if (ifFirstTime())
        UART_puts("System started\n");
    else
        UART_puts("System rebooted\n");

    setNotFirstTime();

    state = SW2;
    LED = 1;

    while(1)
    {
        if (state != SW2)
        {   
            if (state==1)
            {
                UART_puts("Switch depressed\n");
                LED = ! LED;
                if (LED==1)
                {
                        UART_puts("LED is now on\n");
                    } else {
                        UART_puts("LED is now off\n");
                }
            } else {
                UART_puts("Switch released\n");
            }
            state = SW2;
        }
    }
}
