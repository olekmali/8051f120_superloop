// Copyright (C) 2017-2017 Aleksander Malinowski

#include "C8051F120.h"                         // Device-specific SFR Definitions
#include "C8051F120_io.h"                      // Device-specific SFR Definitions

#include "bu_init.h"

#include <stdint.h>

void main(void)
{
    // make sure that unnecessary computations are not optimized out - use volatile
    volatile uint8_t  var_u8  = 1;
    volatile int8_t   var_i8  = 1;
    volatile uint16_t var_u16 = 1;
    volatile int16_t  var_i16 = 1;
    volatile uint32_t var_u32 = 1;
    volatile int32_t  var_i32 = 1;
    volatile float    var_flt = 1.0F;
    uint8_t x = 1; // fake input variable

    // Disable watchdog timer
    WDTCN = 0xde;
    WDTCN = 0xad;

    // Initialize the MCU
    PORT_Init();
    SYSCLK_Init();

    LED = 1;

    while(1)
    {
        // do the computation 1000 times so that the speed can be inspected with the naked eye
        uint32_t i;
        for (i=0; i<100000L; i++) {
            x--; if (x==0) x =16; // update for the fake input vsriable
            // we avoid using int *2^n or /2^n as those can be further optimized

            // uncomment only one of the lines below at a time
            // var_u8  = (var_u8  * 7U + x * 3U) / 10U;
            // var_i8  = (var_i8  * 7  + x * 3) / 10;
            // var_u16 = (var_u16 * 7U + x * 3U) / 10U;
            // var_i16 = (var_i16 * 7  + x * 3) / 10;
            // var_u32 = (var_u32 * 7U + x * 3U) / 10U;
            // var_i32 = (var_i32 * 7  + x * 3) / 10;
            // var_flt = var_flt * 0.7 + i * 0.3;
        }
        // evaluate the frequency of blinking for each of the cases: float, 32-bit, 16-bit, and 8-bit unsigned/signed integer
        LED = ! LED;
    }
}
