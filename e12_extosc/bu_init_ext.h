#ifndef _BU_INIT_EXT
#define _BU_INIT_EXT

#define SYSCLK       66355200L      // System clock in Hz (adjust to reflect what you already did in bu_init.c)

// Initialization Routines
void PORT_Init(void);
void SYSCLK_Init(void);

#endif
