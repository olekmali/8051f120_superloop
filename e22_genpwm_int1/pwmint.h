#ifndef _PWMINT1
#define _PWMINT1
#include <stdint.h>

#define NUM_PWM_CHANNELS    (6)
#define NUM_PWM_100_MAX     (255)   // maximum PWM resolution

typedef enum PWMstate { OFF, ON } PWMstate;

void Timer4_PWM_Init        (uint32_t sysclock, uint32_t pwmfrq, uint8_t resolution);

void Timer4_PWM_SetOn       (uint8_t channel, PWMstate newstate);         // 0..5, 0 or 1
void Timer4_PWM_SetDuty     (uint8_t channel, uint8_t newdutypercentage);     // 0..5, 0..100

void Timer4_PWM_ISR         (void) __interrupt 16 __using 3;

#endif
