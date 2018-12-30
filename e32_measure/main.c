#include <C8051F120.h>
#include "C8051F120_io.h"

#include "bu_init.h"
#include "bu_wait.h"
#include "bu_uart.h"
#include "bu_com.h"
#include "adc0int.h"

#include <stdint.h>
#include <stdio.h>

#define BAUDRATE         9600U          // Baud rate of UART in bps
#define INTERRUPT_RATE  50000UL         // Sample frequency in Hz
#define AVG_CNT             8U          // 2^how many == samples should be averaged

void main(void)
{
    // Disable watchdog timer
    WDTCN = 0xde;
    WDTCN = 0xad;

    PORT_Init ();
    SYSCLK_Init();
    UART_Init(SYSCLK, BAUDRATE);

    ADC0_Timer3_Init(SYSCLK, INTERRUPT_RATE, AVG_CNT); 
    // Needs global interrupts enabled to work

    EA = 1;                             // Enable global interrupts

    while (1)
    {
        uint16_t temperature;               // temperature in hundredths of a degree C
        uint16_t temp_int, temp_frac;       // integer and fractional portions of temperature
        __xdata char buffer[80];            // character buffer for outputting temperature

        wait_ms(SYSCLK, 500);           // wait 500 milliseconds

        // Read the latest A/D results
        temperature = getRecentResult();
    
        // calculate temperature in tenth of a degree
        temperature = temperature - 42380;
        temperature = (temperature * 10) / 156;
        temp_int = temperature / 10;
        temp_frac = temperature - (temp_int * 10);

        sprintf (buffer, "The temperature is %+02d.%1d\n", temp_int, temp_frac);
        UART_puts(buffer);
    
        if (temp_int<30)
        {
            LED = 0;
        } else  if (temp_int<36){
            LED = ! LED;
        } else {
            LED = 1;
        }    
    }

}
