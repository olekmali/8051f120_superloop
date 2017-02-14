#ifndef _ADC0DSP
#define _ADC0DSP
#include <stdint.h>

uint16_t  getRecentInput(uint8_t channel);
void      setNextOutput (uint8_t channel, uint16_t value);

// a very simple semaphore
uint8_t   is_data_ready();
void      reset_data_ready();

void ADC0_DACs_Timer3_Init (uint32_t sysclock, uint32_t rate);
void Timer3_ISR (void)  __interrupt 14; 
void ADC0_ISR (void)    __interrupt 15 __using 3;

// Note: required:         1 <= sysclock/12/rate   <= 65535
// Note: if frequency matters:   sysclock/12/rate   should be integer
// Note: in real life applications keep ^^^^^^^^   as low as practical

#endif
