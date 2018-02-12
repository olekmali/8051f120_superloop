#include "C8051F120.h"
#include "C8051F120_io.h"
#include "adc0ctrl8.h"

#include "bu_init.h"
#include "bu_wait.h"
#include "bu_uart.h"
#include "bu_com.h"

#include <stdint.h>
#include <stdio.h>

#define BAUDRATE         9600U          // Baud rate of UART in bps
#define INTERRUPT_RATE   1000UL         // Sample frequency in Hz
#define TEMP_CHANNEL    8

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

    // Note: the control loop is in the Interrupts
    while (1)
    {
        uint16_t temperature;           // temperature in hundredths of a degree C
        uint16_t temp_int, temp_frac;   // integer and fractional portions of temperature
        __xdata char buffer[80];        // character buffer for outputting temperature

        // Take the measurement and read the A/D result
        temperature = getRecentInput(TEMP_CHANNEL);
        // Note: Channel 8 is measured with gain of 2, all others with 1
        // needs to be compensated here at the cost of lower resolution
    
        // calculate temperature in tenth of a degree
        EA=0;   // * and / for int16_t/int32_t are not reentrant in the default memory model
                // we use them also in the control loop interrupt
        EA=0;
        temperature = temperature - 42380;
        temperature = (temperature * 10) / 156;
        temp_int = temperature / 10;
        temp_frac = temperature - (temp_int * 10);
        EA=1;

        sprintf (buffer, "CTRL loop is running, T: %+02d.%1d\n", temp_int, temp_frac);
        UART_puts(buffer);
    
        wait_ms(SYSCLK, 1000);                   // wait 1000 milliseconds
    }

}
