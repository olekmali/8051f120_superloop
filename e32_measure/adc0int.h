#ifndef _ADC0INT
#define _ADC0INT
#include <stdint.h>

void ADC0_Timer3_Init (uint32_t sysclock, uint32_t rate, uint8_t avgfactor);
void ADC0_ISR (void) __interrupt 15;
uint16_t getRecentResult();

#endif
