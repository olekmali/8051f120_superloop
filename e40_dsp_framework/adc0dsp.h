#ifndef _ADC0DSP
#define _ADC0DSP
#include <stdint.h>

// Note: required:         1 <= sysclock/12/rate   <= 65535
// Note: if frequency matters:   sysclock/12/rate   should be integer
// Note: in real life applications keep ^^^^^^^^   as low as practical

void ADC0_DACs_Timer3_Init (uint32_t sysclock, uint32_t rate);
void ADC0_ISR (void) __interrupt 15 __using 3;

#endif
