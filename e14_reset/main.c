#include "C8051F120.h"                  // Device-specific SFR Definitions
#include "C8051F120_io.h"               // Device-specific SFR Definitions

#include "bu_init.h"
#include "bu_uart.h"
#include "bu_com.h"
#include "bu_reset.h"

#include <stdint.h>

#define BAUDRATE     9600U              // Baud rate of UART in bps

void main(void)
{
    uint8_t state;


    // Disable watchdog timer
    WDTCN = 0xde;
    WDTCN = 0xad;

    // Initialize the MCU
    PORT_Init();
    SYSCLK_Init();
    UART_Init(SYSCLK, BAUDRATE);

    state = getResetSource();
    if ( 0 != state & RESET_BY_EXTRST ) UART_puts("Reset source: External Reset\n");
    if ( 0 != state & RESET_BY_PWRLEV ) UART_puts("Reset source: Power Level\n");
    if ( 0 != state & RESET_BY_MISCLK ) UART_puts("Reset source: Missing Clock\n");
    if ( 0 != state & RESET_BY_WCHDOG ) UART_puts("Reset source: Watchdog\n");
    if ( 0 != state & RESET_BY_INTRST ) UART_puts("Reset source: Internal Software Reset\n");
    if ( 0 != state & RESET_BY_CMPRST ) UART_puts("Reset source: Comparator\n");
    if ( 0 != state & RESET_BY_CNVRST ) UART_puts("Reset source: Converter\n");

    UART_puts("End of the demo. Press RESET to try again.\n");

    while(1)
        ;
}
