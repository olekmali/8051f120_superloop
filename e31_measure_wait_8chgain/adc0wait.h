#ifndef _ADC0INT
#define _ADC0INT
#include <stdint.h>

#define         ADC0_MAX    (0xFFF0)
#define         VREF          (2400)

#define         ADC0_CH_TEMP     (8)
#define         ADC0_CH_MAX      (8)
#define         ADC0_GAIN_HALF  (64)
#define         ADC0_GAIN_ONE    (1)
#define         ADC0_GAIN_MAX   (16)

void     ADC0_Wait_Init(uint32_t sysclock);
void     setGain(uint8_t gain);
void     setChannel(uint8_t channel);
uint16_t getADC0(); // will wait for the current result

#endif
