#ifndef _TIMER3INT
#define _TIMER3INT
#include <stdint.h>

#include "test_cfg.h"                   // control which experiment to run

void Timer3_Init(uint32_t sysclock, uint32_t rate, uint16_t rate2);


#ifdef use_function_to_change_parameter
    void Timer3_setParam(uint8_t onoff);
    uint8_t Timer3_getParam(void);
#else
    #ifndef force_volatile_parameter_variable
        extern uint16_t Timer3_parameter;
    #else
        extern volatile uint16_t Timer3_parameter;
    #endif
#endif
    


#ifdef SDCC
    void Timer3_ISR (void) __interrupt 14; 
#endif

#endif
