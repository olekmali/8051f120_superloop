#include "C8051F120.h"
#include "adc0dsp.h"

#include "bu_init.h"
#include "bu_wait.h"
#include "bu_uart.h"
#include "bu_com.h"

#include <stdint.h>
#include <stdio.h>

#define BAUDRATE        9600U           // Baud rate of UART in bps
#define INTERRUPT_RATE  8000UL          // Sample frequency in Hz

void main(void)
{
    // Disable watchdog timer
    WDTCN = 0xde;
    WDTCN = 0xad;

    PORT_Init ();
    SYSCLK_Init();
    UART_Init(SYSCLK, BAUDRATE);

    ADC0_DACs_Timer3_Init(SYSCLK, INTERRUPT_RATE);
    // Needs global interrupts enabled to work

    EA = 1;                             // Enable global interrupts

    while (1)
    {
        UART_puts("Still alive\n");
        wait_ms(SYSCLK, 1000);          // wait 1000 milliseconds
    }

}
