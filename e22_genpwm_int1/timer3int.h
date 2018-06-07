#ifndef _TIMER3INT
#define _TIMER3INT
#include <stdint.h>

// Note: required:         1 <= sysclock/12/rate   <= 65535
// Note: if frequency matters:   sysclock/12/rate   should be integer
// Note: in real life applications keep ^^^^^^^^   as low as practical

void    Timer3_Init (uint32_t sysclock, uint32_t rate, uint32_t rate_pwm);
void    Timer3_setRate(uint8_t new_rate);


#ifdef SDCC
void Timer3_ISR (void) __interrupt 14; 
    // SDCC - interrupt should have function prototypes that are included in main.c
    // KEIL - interrupt must not have function prototypes
    #endif

#endif
