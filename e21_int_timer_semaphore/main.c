#include "bu_init.h"
#include "timer3int.h"
#include "c8051F120.h"
#include "c8051F120_io.h"

#define INTERRUPT_RATE  50000UL     // Interrupt frequency in Hz
#define LOOP_RATE           4U      // Loop semaphore frequency in Hz

void main(void)
{
    // Disable watchdog timer
    WDTCN = 0xde;
    WDTCN = 0xad;

    PORT_Init ();
    SYSCLK_Init();

    Timer3_Init(SYSCLK, INTERRUPT_RATE, LOOP_RATE);
 
    EA = 1;                         // enable global interrupts

    while (1)
    {
        // wait for Timer3 interrupt, when it ends
        while( !Timer3_GetSemaphore() ) ;
        // and immediately afterwards reset the interrupt done marker for the next time
        Timer3_ResetSemaphore();

        LED = !LED;
    }
}
