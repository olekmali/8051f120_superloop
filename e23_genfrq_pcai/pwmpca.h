#ifndef _PWMPCA
#define _PWMPCA
#include <stdint.h>

typedef enum PWMstate { OFF, ON } PWMstate;

void PCA0_PWM_Init(uint32_t sysclk);

void PCA0_PWM_SetOn       (uint8_t channel, PWMstate newstate);               // 0..5, 0 or 1
void PCA0_PWM_SetOffset   (uint8_t channel, uint16_t  deltaoffset);       // 0..360
void PCA0_PWM_SetFrequency(uint8_t channel, uint32_t newfrequency);  // 0..5, ??..?? - frequency range depends on PCA0 source

void PCA0_PWM_ISR         (void) __interrupt 9 __using 3;

#endif
