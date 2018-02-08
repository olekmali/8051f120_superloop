#ifndef _BU_RESET
#define _BU_RESET

#include <stdint.h>

uint8_t getResetSource();
void    doForceFullReset();
void    doForceSoftReset();

#define RESET_BY_EXTRST    0x01
#define RESET_BY_PWRLEV    0x02
#define RESET_BY_MISCLK    0x04
#define RESET_BY_WCHDOG    0x08
#define RESET_BY_INTRST    0x10
#define RESET_BY_CMPRST    0x20
#define RESET_BY_CNVRST    0x40

#endif
