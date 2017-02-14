#ifndef _ADC0INTRAPID
#define _ADC0INTRAPID
#include <stdint.h>

#define         ADC0_MAX    (0xFFF0)
#define         VREF          (2400)

#define         ADC0_CH_TEMP     (8)
#define         ADC0_CH_MAX      (8)
#define         ADC0_GAIN_HALF  (64)
#define         ADC0_GAIN_ONE    (1)
#define         ADC0_GAIN_MAX   (16)

void      setGain(uint8_t gain);
uint16_t  getRecentResult(uint8_t channel);

// a very simple semaphore
uint8_t   is_data_ready();

void      ADC0_Timer3_Init (uint32_t sysclock, uint32_t rate);
void      Timer3_ISR (void)  __interrupt 14; 
void      reset_data_ready();
void      ADC0_ISR (void) __interrupt 15;

// Note: required:         1 <= sysclock/12/rate   <= 65535
// Note: if frequency matters:   sysclock/12/rate   should be integer
// Note: in real life applications keep ^^^^^^^^   as low as practical

// Note: rate << 200kHz/9 as we need to service 9 ADC channels between Timer3 overflows

#endif
