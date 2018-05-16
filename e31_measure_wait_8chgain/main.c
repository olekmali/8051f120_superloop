// Copyright (C) 2008 Mike Firman
#include <C8051F120.h>
#include "C8051F120_io.h"
#include "adc0wait.h"
#include "bu_init.h"
#include "bu_uart.h"
#include "bu_com.h"
#include "bu_wait.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define BAUDRATE         9600U          // Baud rate of UART in bps
#define BUFLEN            160


// comment the line below to disable the gain auto-adjustment
#define AUTO_ADJUST_GAIN 1


void main(void)
{
    __xdata char buffer[BUFLEN+1];

    uint16_t    ADCReading[ADC0_CH_MAX];
    uint8_t     gain[ADC0_CH_MAX]={1,1,1,1,1,1,1,1};
    uint16_t    voltage[ADC0_CH_MAX];
    uint8_t     inputchannel=0;

    // Disable watchdog timer
    WDTCN = 0xde;
    WDTCN = 0xad;

    PORT_Init ();
    SYSCLK_Init();
    UART_Init(SYSCLK, BAUDRATE);

    ADC0_Wait_Init(SYSCLK);
    setChannel(0);
    setGain(1); // this gain applies to all channels

    while (1)
    {
        // set/update gain for the current channel
        setGain(gain[inputchannel]); // this gain applies to all channels

        // read the current channel
        ADCReading[inputchannel] = getADC0();

        if (SW2)
        {
            // adjust gain and re-read as necessary
            if( ADCReading[inputchannel]<=0x7F00 )
            {
                if (gain[inputchannel]==ADC0_GAIN_HALF)
                {
                    gain[inputchannel]=ADC0_GAIN_ONE;
                    continue;
                }
                else if (gain[inputchannel]<ADC0_GAIN_MAX)
                {
                    gain[inputchannel]=gain[inputchannel]<<1;
                    continue;
                }
            }
            else if ( ADCReading[inputchannel]>=0xFF00 )
            {
                if (gain[inputchannel]>ADC0_GAIN_ONE) {
                    gain[inputchannel]=gain[inputchannel]>>1;
                    continue;
                }
                else if (gain[inputchannel]==ADC0_GAIN_ONE)
                {
                    gain[inputchannel]=ADC0_GAIN_HALF;
                    continue;
                }
            }
        } else {
            if (gain[inputchannel]!=ADC0_GAIN_ONE)
            {
                gain[inputchannel]==ADC0_GAIN_ONE;
                continue;
            }
        }

        // compute the voltage
        EA=0;
        if(gain[inputchannel]!=ADC0_GAIN_HALF)
        {
            voltage[inputchannel] = (int32_t)ADCReading[inputchannel]*(uint16_t)VREF/gain[inputchannel]/ADC0_MAX;
        }
        else // gain of 0.5
        {
            voltage[inputchannel] = (int32_t)ADCReading[inputchannel]*((uint8_t)2*(uint16_t)VREF)/ADC0_MAX;
        }
        EA=1;

        // cycle through channels
        inputchannel=inputchannel+1;
        if(inputchannel==ADC0_CH_MAX)
        {
            inputchannel=0;
        }
        setChannel(inputchannel);

        // print the measurements when the complete set taken
        if (inputchannel==0)
        {

            EA=0;
            sprintf(buffer, "\rV%4u %4u %4u %4u %4u %4u %4u %4u G%3u %3u %3u %3u %3u %3u %3u %3u",
                    voltage[0],voltage[1],voltage[2],voltage[3],voltage[4],voltage[5],voltage[6],voltage[7],
                    gain[0],gain[1],gain[2],gain[3],gain[4],gain[5],gain[6],gain[7]
                    );
            EA=1;
            UART_puts(buffer);
        }

        wait_ms(SYSCLK, 20);
    }
}
