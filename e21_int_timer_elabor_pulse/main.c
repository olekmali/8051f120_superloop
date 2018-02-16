

#include "c8051F120.h"
#include "c8051F120_io.h"

#include "bu_init.h"
#include "bu_watchdog.h"
#include "timer3int.h"

#include <stdint.h>

// Frequencies in Hz
#define INTERRUPT_RATE    1000UL    // Interrupt frequency in Hz
#define LOOP_RATE          100U     // Loop semaphore frequency in Hz (100Hz == 10ms)

void main(void)
{
    uint8_t  state;
    uint16_t led_active = 0;

    // Set up watchdog timer (10.4ms at sysclk of 100MHz)
    WatchDog_set_10ms();

    PORT_Init ();
    SYSCLK_Init();

    Timer3_Init(SYSCLK, INTERRUPT_RATE, LOOP_RATE);
    EA = 1;                         // enable global interrupts

    LED = 0;
    state = SW2;
    while (1)
    {
        // wait for Timer3 interrupt, at the rate of LOOP_RATE
        // it will release the semaphore so that the loop does one iteration
        while( !Timer3_GetSemaphore() ) {
            WatchDog_reset();   // not every interrupt releases the semaphore
                                // reset the watchdog timer while still waiting
            
            // sleep until the next interrupt happens and then check [again]
            // if it is the one we are waiting for to be completed
            CPU_sleep();
        }
        // and immediately afterwards reset the interrupt done marker for the next time
        Timer3_ResetSemaphore();

        // Reset the watchdog timer
        WatchDog_reset();

        // code below is executed at the frequency of LOOP_RATE 
        // all timing is expressed by loop counts times (1000ms/LOOP_RATE)
        // in this example the loop rate is 100 and hence the time period is 10ms
        // **********************************************************************

        if (0==SW2)
        {   
            if (led_active==0) led_active=300;
        }

        if (led_active>0)
        {
            if (led_active%50==0)
                LED = !LED;
            led_active--;
        }
    }
}
