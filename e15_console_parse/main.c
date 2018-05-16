#include <C8051F120.h>
#include "C8051F120_io.h"
#include "bu_init.h"
#include "bu_uart.h"
#include "bu_com.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/*
    Function split_by_comma
        input:  a char array with ASCII data in the format of comma-delimited columns of data
        output: array of extracted int32_teger values of each numeric data column up to the maximum columns limit
        input:  maximum column limit that also specifies maximum allowed size for the array of extracted numeric data
        return: actual number of extracted columns of numeric data
*/
uint8_t split_by_comma(const char data_string[], int32_t data_extracted[], uint8_t max_columns)
{
    uint8_t position    = 0;
    uint8_t columns_cnt = 1;
    
    data_extracted[0]=atol(&data_string[0]);    

    while(data_string[position]!='\0' && columns_cnt<max_columns)
    {
        if (data_string[position]==',') {
            data_extracted[columns_cnt]=atol(&data_string[position+1]);    
            columns_cnt++;
        }
        position++;
    }
    return(columns_cnt);
}

#define BAUD_RATE               9600U
#define MAX_GPS_LINE_LENGTH       80
#define MAX_DATA_COLUMNS          20

void main(void)
{
    __xdata char buffer[MAX_GPS_LINE_LENGTH];   // char string data received from GPS
    __xdata int32_t numbers[MAX_DATA_COLUMNS];     // extracted columns of data
    __xdata char buf_test[16];                  // variable for test printouts
    uint8_t received_cnt, i;

    // Disable watchdog timer
    WDTCN = 0xde;
    WDTCN = 0xad;

    // Initialize the MCU
    PORT_Init();
    SYSCLK_Init();
    UART_Init(SYSCLK, BAUD_RATE);

    while(1)
    {
        // Enter a test string -- this string would be received by UART connected to a GPS device
        UART_puts("Please enter a line comma-delimited data\n");
        UART_gets(buffer, sizeof(buffer));

        // This is the function that we are testing
        received_cnt = split_by_comma(buffer, numbers, MAX_DATA_COLUMNS);

        // This string is printed to a console instead of using data for number crunching
        sprintf(buf_test, "%d items:", received_cnt);
        UART_puts(buf_test);
        
        for (i=0; i<received_cnt; i++) {
            sprintf(buf_test, " %ld", numbers[i]);
            UART_puts(buf_test);
        }
        UART_puts(" Done\n");
    }
}
