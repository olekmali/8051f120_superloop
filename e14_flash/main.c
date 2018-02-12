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
 
    if (ifFirstTime()) {
        UART_puts("System started\n");
        setNotFirstTime();
    } else {
        UART_puts("System rebooted\n");
    }

    UART_puts("End of the demo. Press RESET to try again.\n");

    while(1)
        ;
}
