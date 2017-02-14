#ifndef _PERIODPCA
#define _PERIODPCA
#include <stdint.h>

void PCA0_Init();

void PCA0_ISR(void) interrupt 9 __using 3;

uint16_t getRecentPeriod();
uint16_t getAveragedPeriod();

#endif
