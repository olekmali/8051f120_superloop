#ifndef _PWMPCA
#define _PWMPCA
#include <stdint.h>

typedef enum PWMstate { OFF, ON } PWMstate;

void PCA0_Init();

void PCA0_SetOn       (uint8_t channel, PWMstate newstate);         // 0..5, 0 or 1
void PCA0_SetDuty     (uint8_t channel, uint8_t newduty);     // 0..5, 0..100

void PCA0_ISR         (void) __interrupt 9 __using 3;

#endif
