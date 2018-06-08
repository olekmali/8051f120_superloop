#include <C8051F120.h>
#include "C8051F120_io.h"

#include "bu_init.h"
#include "bu_watchdog.h"
#include "timer3int.h"
#include "bu_uart.h"
#include "bu_com.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define INTERRUPT_FRQ   5000UL          // Interrupt frequency in Hz
#define PWM_FRQ          250U           // PWM frequency
#define SEMAPHORE_FRQ      4U           // Semaphore frequency
#define KILLSW_FRQ         1U           // Frequency of PWM update that keeps PWM running
#define BAUDRATE        9600U           // Baud rate of UART in bps

#define BUFLEN             20           // Maximum user buffer size

void main(void)
{
    // Disable watchdog timer
    WatchDog_disable_all();
    // it is impossible to use watchdog in a loop when human user provides data 
    // at irregular intervals of time without use of asynchronous input methods

    PORT_Init ();
    SYSCLK_Init();

    Timer3_Init(SYSCLK, INTERRUPT_FRQ, PWM_FRQ, SEMAPHORE_FRQ, KILLSW_FRQ);
    UART_Init(SYSCLK, BAUDRATE);
    EA = 1;                             // enable global interrupts

    while(1)
    {
        __xdata char buffer[BUFLEN+1];
        uint32_t value;

        while( ! semaphore_get() )      // waiting for the next 10ms to start
            ;
        semaphore_reset();

        UART_puts("Enter PWM duty cycle >");
        UART_gets(buffer, sizeof(buffer));
        value = atoi(&buffer[0]);
        if (100U>=value) Timer3_setPWMDuty( (uint8_t)value );
        else UART_puts("PWM duty out of range\n");
    }
}
