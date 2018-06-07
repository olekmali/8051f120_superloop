#ifndef _BU_INIT
#define _BU_INIT

#define SYSCLK       73500000L      // System clock in Hz (adjust to reflect what you already did in bu_init.c)

// Initialization Routines
void PORT_Init(void);
void SYSCLK_Init(void);

#endif
