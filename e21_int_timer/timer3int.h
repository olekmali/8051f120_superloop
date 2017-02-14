#ifndef _TIMER3INT
#define _TIMER3INT
#include <stdint.h>

// Note: required:         1 <= sysclock/12/rate   <= 65535
// Note: if frequency matters:   sysclock/12/rate   should be integer
// Note: in real life applications keep ^^^^^^^^   as low as practical


void Timer3_Init(uint32_t sysclock, uint32_t rate);
void Timer3_setOffOn(uint8_t onoff);
void Timer3_setRates(uint16_t rate1, uint16_t rate2);

uint8_t Timer3_getOffOn(void);

#ifdef SDCC
void Timer3_ISR (void) __interrupt 14; 
    // SDCC - interrupt should have function prototypes that are included in main.c
    // KEIL - interrupt must not have function prototypes
    #endif

#endif
