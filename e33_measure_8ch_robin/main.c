#include "adc0int8rr.h"
#include "C8051F120.h"

#include "bu_init.h"
#include "bu_wait.h"
#include "bu_uart.h"
#include "bu_com.h"

#include <stdint.h>
#include <stdio.h>

#define BAUDRATE       9600U            // Baud rate of UART in bps
#define INTERRUPT_RATE 10000UL          // Sample frequency in Hz

void main(void)
{
    // Disable watchdog timer
    WDTCN = 0xde;
    WDTCN = 0xad;

    PORT_Init ();
    SYSCLK_Init();
    UART_Init(SYSCLK, BAUDRATE);

    ADC0_Timer3_Init(SYSCLK, INTERRUPT_RATE);
    // Needs global interrupts enabled to work
    // Note: the actual refresh rate is INTERRUPT_RATE/number_of_scanned_channels (which is 50/9 KHz here)
    // setGain(1);

    EA = 1;                             // Enable global interrupts
    while (1)
    {
        __xdata char buffer[16];        // character buffer for outputting one formatted value
        uint8_t ch;
        for(ch=0; ch<8; ch++)
        {
            // Read the latest A/D results
            uint16_t value = getRecentResult(ch);
            uint16_t converted = (int32_t)value * (uint16_t)VREF / ADC0_MAX;

            sprintf(buffer, " ch%1d:%4d", ch, converted );  // note: !! formatting works only on xdata buffer !!

            UART_puts(buffer);
            if (ch==3||ch==7) UART_puts("\n");
        }

        {
            uint16_t temperature;           // temperature in hundredths of a degree C
            uint16_t temp_int, temp_frac;   // integer and fractional portions of temperature

            temperature = getRecentResult(8) <<1; // compensate for gain==1 instead of ==2

            // calculate temperature in tenth of a degree
            temperature = temperature - 42380;
            temperature = (temperature * 10) / 156;
            temp_int = temperature / 10;
            temp_frac = temperature - (temp_int * 10);
    
            sprintf (buffer, " T:%+6d.%1d\n", temp_int, temp_frac);    
            UART_puts(buffer);
        }

        UART_puts("\n");
        wait_ms(SYSCLK, 1000);                  // wait 1000 milliseconds
    }

}
