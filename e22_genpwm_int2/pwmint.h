#ifndef _PWMINT
#define _PWMINT
#include <stdint.h>

void Timer4_PWM_Init        (uint32_t sysclock, uint32_t rate);
void Timer4_PWM_SetFrequency(uint32_t newfrequency);
void Timer4_PWM_SetDuty     (uint8_t newduty);

void Timer4_PWM_ISR         (void) __interrupt 16 __using 3;

#endif
