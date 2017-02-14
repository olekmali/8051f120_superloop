#ifndef _INTERRUPT
#define _INTERRUPT
#include <stdint.h>

void Timer4_Init (uint32_t sysclock, uint32_t rate);

void Timer4_ISR  (void) __interrupt 16;

#endif
