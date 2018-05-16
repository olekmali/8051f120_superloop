//* ECE471/571 Homework 2 File - provided test program for hw_sysclk.c

#include <C8051F120.h>
#include "C8051F120_io.h"

#include "hw_sysclk.h"

#include <stdint.h>

#define MODE_MAX (13)
__code const uint8_t array_test_sequences[MODE_MAX] =
    {
        SYSCLK_INT_3062KHz,
        SYSCLK_INT_73500KHz,
        SYSCLK_INT_6125KHz,
        SYSCLK_INT_12250KHz,
        SYSCLK_INT_49000KHz,
        SYSCLK_INT_98000KHz,
        SYSCLK_INT_24500KHz,
        SYSCLK_EXT_22118KHz,
        SYSCLK_INT_12250KHz,
        SYSCLK_EXT_44237KHz,
        SYSCLK_INT_73500KHz,
        SYSCLK_EXT_66355KHz,
        SYSCLK_EXT_88474KHz
        // mode no change in this sequence above
        // you can add more transitions below for further testing
    };

void wait_ms(uint32_t ms)
{
    while (ms>0)
    {   // the loop below takes about 1ms to execute 
        // assuming SYSCLK=24.5MHz and no interrupts
        volatile uint32_t i;
        for (i=400L; 0<i; --i) ;
        ms--;
    }
}

void main(void)
{
    uint8_t mode = 0;

    // Disable watchdog timer
    WDTCN = 0xde;
    WDTCN = 0xad;

    SFRPAGE = CONFIG_PAGE;
    OSCICN  = 0x83;                 // Initially run at 24.5MHz
    XBR2    = 0x40;                 // Enable the crossbar and week pull up resistors
    P1MDOUT|= 0x40;                 // Set P1.6(TB_LED) to push-pull

    LED = 1;

    while(1)
    {
        if (0 == SW2) // button depressed
        {   
            // wait to de-bounce
            wait_ms(20);    // this would be still 5ms at 100MHz
            // wait until button released
            while (0 == SW2) ;
            ++mode;
            if (MODE_MAX == mode) mode=0;

            reinit_sysclk(array_test_sequences[mode]);
        }
        
        LED = !LED;
        wait_ms(1000);
    }
}
