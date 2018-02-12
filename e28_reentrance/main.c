#include "C8051F120.h"
#include "C8051F120_io.h"

#include "bu_init.h"
#include "bu_uart.h"
#include "bu_com.h"
#include "interrupt.h"
#include "bu_wait.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define BAUDRATE         9600U          // Baud rate of UART in bps
#define INTERRUPT_RATE  50000UL         // Interrupt frequency in Hz - high to accommodate high range of PWM frequencies

void main(void)
{
    uint32_t errors = 0, correct = 0;
    int32_t a=1, b=1, c, d;
    __xdata static char buffer[64];

    // Disable watchdog timer
    WDTCN = 0xde;
    WDTCN = 0xad;

    PORT_Init ();
    SYSCLK_Init();
    UART_Init(SYSCLK, BAUDRATE);

    // using Timer4 as update scheduler initialize T4 to update DAC1 after (SYSCLK cycles)/sample have passed.
    Timer4_Init (SYSCLK, INTERRUPT_RATE);
    EA = 1;

    a = 654321;
    b = 123456;
    while (1) {
        // below you can find the same division encoded in two different ways
        // hopefully the compiler optimization is unable to see and optimize that

        EA = 0; // <- comment this and you will see errors detected
        c = a / b;
        a = a * 10;
        b = b * 10;        
        d = a / b;
        a = a / 10;
        b = b / 10;
        EA = 1;

        if (c != d) errors++; else correct++;
        // note: this above is just for quick testig as the variables will eventually overflow and roll over

        EA = 0; // <- comment this and you may see program hangs on sprintf!!
        sprintf(buffer,"err: %lu ok: %lu              \r", errors, correct);
//      sprintf(buffer,"a: %ld b: %ld c: %ld d: %ld   \r", a, b, c, d);
        EA = 1;

        UART_puts(buffer);
        wait_ms(SYSCLK, 50);    // this is line is to prevent buggy Windows UART-USB device drivers
                                // from crashing when continuous stream of data is received from uC
    }

}
