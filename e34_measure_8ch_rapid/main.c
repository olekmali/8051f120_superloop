#include "adc0int8rap.h"
#include <C8051F120.h>
#include "C8051F120_io.h"

#include "bu_init.h"
#include "bu_wait.h"
#include "bu_uart.h"
#include "bu_com.h"

#include <stdint.h>
#include <stdio.h>

#define BAUDRATE         9600U          // Baud rate of UART in bps
#define INTERRUPT_RATE    100UL         // Sample frequency in Hz

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
    // Note: all channels are sampled at this rate almost simultaneously
    // setGain(1);

    EA = 1;                             // Enable global interrupts

    SFRPAGE = CONFIG_PAGE;
    while (1)
    {
        __xdata char buffer[16];        // character buffer for outputting one formatted value
        int16_t ch;

/* */
        // this IF statement is for diagnostic purposes only
        if (is_data_ready())
        {
            // we run over the time allowed between samplings
            // we should have been waiting for the next sample to become available
            AB4_LED2 = 1;               // set the RED error indicator
        } else {
            AB4_LED2 = 0;               // reset the RED error indicator in case it was set
            // wait until the next sampling cycle
        }
/* */
 
        // THIS LOOP TIMING CONTROL WILL NOT WORK IF YOU USE UART 
        // at low transmission speed as it does take forever 
        // (i.e. about 1ms per character) to transmit data
        while (! is_data_ready() ) ;
        reset_data_ready();

        for(ch=0; ch<8; ch++)
        {
            // Read the latest A/D results
            uint16_t value = getRecentResult(ch);
            uint16_t converted = (int32_t)value * (uint16_t)VREF / ADC0_MAX;

            sprintf(buffer, " ch%1d:%4d", ch, converted );  // note: !! formatting works only on xdata buffer !!

            if (!SW2)
            {
                UART_puts(buffer);
                if (ch==3||ch==7) UART_puts("\n");
            }
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
    
            if (!SW2)
            {
                sprintf (buffer, " T:%+6d.%1d\n", temp_int, temp_frac);    
                UART_puts(buffer);
                UART_puts("\n");
            }
        }

//      if (!SW2)
//      {
//          wait_ms(SYSCLK, 1000); // since we already use UART we may as well wait much int32_ter for humans to read
//      }
    }

}
