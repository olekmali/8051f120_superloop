#include "C8051F120.h"
#include "C8051F120_io.h"

#include "bu_init.h"
#include "bu_wait.h"

void main(void)
{
    // Disable watchdog timer
    WDTCN = 0xde;
    WDTCN = 0xad;

    PORT_Init ();
    SYSCLK_Init();

    LED = 1;

    while (1)
    {
        wait_ms(SYSCLK, 500U);
        LED = ! LED;
    }
}
