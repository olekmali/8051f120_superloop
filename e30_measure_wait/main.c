#include <C8051F120.h>
#include "C8051F120_io.h"

#include "bu_init.h"
#include "bu_wait.h"
#include "bu_uart.h"
#include "bu_com.h"
#include "adc0wait.h"

#include <stdint.h>
#include <stdio.h>

#define BAUDRATE         9600U          // Baud rate of UART in bps
#define TEMP_CHANNEL        8

void main(void)
{
    uint16_t temperature;               // temperature in hundredths of a degree C
    uint16_t temp_int, temp_frac;       // integer and fractional portions of temperature
    __xdata char buffer[80];            // character buffer for outputting temperature

    // Disable watchdog timer
    WDTCN = 0xde;
    WDTCN = 0xad;

    PORT_Init ();
    SYSCLK_Init();
    UART_Init(SYSCLK, BAUDRATE);

    ADC0_Wait_Init(SYSCLK);
    setGain(1);

    while (1)
    {
        // Take the measurement and read the A/D result
        setGain(2);
        setChannel(TEMP_CHANNEL);
        temperature = getADC0();
    
        // calculate temperature in tenth of a degree
        temperature = temperature - 42380;
        temperature = (temperature * 10) / 156;
        temp_int = temperature / 10;
        temp_frac = temperature - (temp_int * 10);

        sprintf (buffer, "The temperature is %+02d.%1dC\n", temp_int, temp_frac);
        UART_puts(buffer);
    
        if (temp_int<30)
        {
            LED = 0;
        } else  if (temp_int<36){
            LED = ! LED;
        } else {
            LED = 1;
        }    

        wait_ms(SYSCLK, 500);           // wait 500 milliseconds
    }

}
