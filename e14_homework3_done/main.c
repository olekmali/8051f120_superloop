#include <C8051F120.h>

#include "bu_init.h"
#include "bu_uart.h"
#include "bu_com.h"
#include "bu_flash.h"

#include <stdint.h>
#include <stdio.h>

#define BAUDRATE         9600U          // Baud rate of UART in bps

typedef struct config_t {
    uint32_t   count;
    char       name[31];
};


void main(void)
{
    struct config_t remember; 
    char buffer[10];


    // Disable watchdog timer
    WDTCN = 0xde;
    WDTCN = 0xad;

    // Initialize the MCU
    PORT_Init();
    SYSCLK_Init();
    UART_Init(SYSCLK, BAUDRATE);
    FLASH_Init();
 
    if (ifFirstTime())
    { // For the first time boot - ask for the name
        UART_puts("What is your name, please? ");
        UART_gets(remember.name, 31);
        remember.count=0;
        setNotFirstTime();
    }
    else
    { // For the returning customers, greet them and ask whether desire to change
        FLASH_get(0, (void*) &remember, sizeof(remember) );
        if (remember.count<0xFFFFFFFF)
            remember.count++;
        UART_puts("\n\nWelcome for the ");
        sprintf(buffer,"%lu", remember.count);
        UART_puts(buffer);
        UART_puts("th time, ");
        UART_puts(remember.name);
        UART_puts("\nWould you like to change the name? (y/n) ");
        UART_gets(buffer,2);
        if (buffer[0]=='y')
        { // Q: Repeated code! Should we use a function with the structure as a parameter?
        UART_puts("What is your name, please? ");
            UART_gets(remember.name, 31);
            remember.count=0;
        }
    }

    FLASH_erase(0);
    FLASH_put(0, (void*) &remember, sizeof(remember) );
    UART_puts("Press RESET to try again!\n");

    while(1)
        ;
}
