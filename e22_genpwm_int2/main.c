#include <C8051F120.h>
#include "C8051F120_io.h"

#include "bu_init.h"
#include "pwmint.h"

#include <stdint.h>

#define INTERRUPT_RATE  50000UL         // Interrupt frequency in Hz - high to accommodate high range of PWM frequencies
#define PWM_FREQUENCY    1000U          // Interrupt frequency in Hz - high to accommodate high range of PWM frequencies

void main(void)
{
    uint8_t state;
    uint8_t rate = 10U;

    // Disable watchdog timer
    WDTCN = 0xde;
    WDTCN = 0xad;

    PORT_Init ();
    SYSCLK_Init();

    // using Timer4 as update scheduler initialize T4 to update DAC1 after (SYSCLK cycles)/sample have passed.
    Timer4_PWM_Init (SYSCLK, INTERRUPT_RATE);
    Timer4_PWM_SetFrequency(PWM_FREQUENCY);
    Timer4_PWM_SetDuty(rate);
    EA = 1;

    state = SW2;
    while(1)
    {
        if (state!=SW2)
        {
            state=SW2;
            if (!SW2)
            {
                rate = rate + 5U;
                if (rate>100U)
                    rate=0U;
                Timer4_PWM_SetDuty(rate);
            }
        }
        // Add some delay if button is not debounced
    }
}
