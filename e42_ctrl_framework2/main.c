#include "C8051F120.h"
#include "C8051F120_io.h"
#include "adc0ctrl8.h"
#include "pwmint.h"

#include "bu_init.h"
#include "bu_uart.h"
#include "bu_com.h"

#include <stdint.h>
#include <stdio.h>

#define CTRL_SAMPLE_RATE       100U     // Control input Sample frequency in Hz
#define PWM_SAMPLE_RATE     200000UL    // PWM interrupt rate
#define PWM_FRQ_RATE         10000U     // PWM frequency in Hz

// extras
#define BAUDRATE     9600U              // Baud rate of UART in bps
//#define BAUDRATE  38400U              // Baud rate of UART in bps
#define TEMP_CHANNEL    8               // which AD channel measures temperature

void main(void)
{
    // Disable watchdog timer
    WDTCN = 0xde;
    WDTCN = 0xad;

    PORT_Init ();
    SYSCLK_Init();
    UART_Init(SYSCLK, BAUDRATE);

    ADC0_DACs_Timer3_Init(SYSCLK, CTRL_SAMPLE_RATE);
    // Needs global interrupts enabled to work

    Timer4_PWM_Init(SYSCLK, PWM_SAMPLE_RATE);
    // Needs global interrupts enabled to work
    Timer4_PWM_SetFrequency(PWM_FRQ_RATE);
    // Timer4_PWM_SetOn       (channel, newstate);
    // Timer4_PWM_SetOn       (channel, newstate);

    EA = 1;                             // Enable global interrupts

    SFRPAGE = CONFIG_PAGE;
    while (1)
    {
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


        while (! is_data_ready() ) ;
        reset_data_ready();

        // x = getRecentInput(channel);
        // setNextOutput (channel, value);
        // Timer4_PWM_SetDuty     (channel, newduty);

        {
            uint16_t temperature;           // temperature in hundredths of a degree C
            uint16_t temp_int, temp_frac;   // integer and fractional portions of temperature
            __xdata char buffer[80];        // character buffer for outputting temperature
    
            temperature = getRecentInput(TEMP_CHANNEL); // Note: Ch8 gain is set to 2, all others 1
            EA=0;   // * and / for int16_t/int32_t are not reentrant in the default memory model
                    // we use them also in the control loop interrupt
            temperature = temperature - 42380;
            temperature = (temperature * 10) / 156;
            temp_int = temperature / 10;
            temp_frac= temperature - (temp_int * 10);
            sprintf (buffer, "T= %+02d.%1dC\n", temp_int, temp_frac);
            EA=1;
    
            // Press SW to enable the transmission below and see timing error red LED
            if (!SW2) UART_puts(buffer);    // WARNING! This takes about 1ms per character!
        }
    }
}
