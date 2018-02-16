#ifndef _BU_WATCHDOG
#define _BU_WATCHDOG

#include "C8051F120.h"

inline void WatchDog_disable_all() {
    WDTCN = 0xde;
    WDTCN = 0xad;
}

inline void WatchDog_set_10ms() {
    WDTCN = 0xa5;   // reset count
    WDTCN = 0xff;   // disable disabling
    WDTCN = 0x07;   // set time out value to maximum (10.4ms @ 100MHz)
}

inline void WatchDog_reset() {
    WDTCN = 0xa5;
}


inline void CPU_sleep() {
    PCON |= 0x01;   // Set IDLE bit
    PCON = PCON;    // ... Followed by a 3-cycle Dummy Instruction
}

#endif
