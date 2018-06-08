#include <C8051F120.h>
#include "C8051F120_io.h"

#include "bu_init.h"
#include "bu_watchdog.h"
#include "timer3int.h"
#include "bu_uart.h"

#include <stdint.h>

#define INTERRUPT_FRQ   5000UL          // Interrupt frequency in Hz
#define PWM_FRQ          250U           // PWM frequency
#define SEMAPHORE_FRQ    100U           // Semaphore frequency
#define KILLSW_FRQ         1U           // Frequency of PWM update that keeps PWM running
#define BAUDRATE        9600U           // Baud rate of UART in bps

void main(void)
{
    // Set up watchdog timer (10.4ms at sysclk of 100MHz)
    WatchDog_set_10ms();

    PORT_Init ();
    SYSCLK_Init();

    Timer3_Init(SYSCLK, INTERRUPT_FRQ, PWM_FRQ, SEMAPHORE_FRQ, KILLSW_FRQ);
    UART_Init(SYSCLK, BAUDRATE);
    EA = 1;                             // enable global interrupts

    while(1)
    {
        while( ! semaphore_get() )      // waiting for the next 10ms to start
            ;
        semaphore_reset();
        WatchDog_reset();               // Reset watchdog timer

        if ( ready_getchar() )
        {
            uint8_t rate = 0;
            rate = (uint8_t)getchar();
            if (rate>100U) 
                rate = 100U;
            Timer3_setPWMDuty(rate);
        }
    }
}
