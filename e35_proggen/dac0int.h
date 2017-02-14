#ifndef _DAC0INT
#define _DAC0INT
#include <stdint.h>

typedef enum Waveform {                   // the different possible output waveforms
    OFF, SQUARE, SINE, TRIANGLE, SAW
} Waveform;


void DAC0_Timer3_Init (uint32_t sysclock, uint32_t rate);
void DAC0_Timer3_SetType(Waveform newtype);
void DAC0_Timer3_SetFrequency(uint16_t newfrequency);
void DAC0_Timer3_SetAmplitude(uint8_t newamplitude);      // 0..100
void DAC0_Timer3_SetOffset   (uint8_t newoffset);         // 0..100
void DAC0_Timer3_ISR (void) __interrupt 14 __using 3;

#endif
