/*
 * hal_UART.c
 *
 *  Created on: 25 mar. 2019
 *      Author: airizar
 */
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ti/drivers/UART.h>
// #include "Board.h"
#include "hal_UART.h"

extern int vsnprintf(char * s,
                     size_t n,
                     const char * format,
                     va_list arg);

static UART_Handle uartHandle;

/*!
 *  @brief  Function to start a UART peripheral
 *
 *  Function to initialize, configure and open a given UART peripheral specified by the
 *  particular index value at a speed given by baudRate value. 8 bits datalength,
 *  no parity and 1 stop bit are fixed parameters
 *
 *  @pre    UART_Startup() has been called
 *
 *  @param  index         Logical peripheral number for the UART indexed into
 *                        the UART_config table
 *
 *  @param  baudRate      UART speed data transfer, use one of the allowed ones
 *
 *  @return A #UART_Handle upon success. NULL if an error occurs, or if the
 *          indexed UART peripheral is already opened.
 *
 */
UART_Handle Startup_UART(uint_least8_t index, uint32_t baudRate) {

//   UART_Handle uart;
    UART_Params uartParams;

    UART_init();
    UART_Params_init(&uartParams);

    uartParams.writeDataMode = UART_DATA_BINARY;
    uartParams.readDataMode = UART_DATA_BINARY;
    uartParams.readReturnMode = UART_RETURN_FULL;
    uartParams.readMode      = UART_MODE_BLOCKING;
    uartParams.readEcho = UART_ECHO_OFF;
    uartParams.baudRate = baudRate;

    uartHandle = UART_open(index, &uartParams);
    if (uartHandle == NULL) {
       /* UART_open() failed */
       while (1);
    }

    return (uartHandle);

}

bool UART_resetInputBuffer(UART_Handle hUart) {
    bool bUartBufferIsAvailable = false;

    // while data are presented in UART buffer
    // we read each available byte to NULL pointer
    do
    {
        if (UART_control(hUart, UART_CMD_ISAVAILABLE, &bUartBufferIsAvailable) < 0)
        {
            // Unexpected error
            return false;
        }
        if (bUartBufferIsAvailable)
        {
            // flush available byte
            UART_read(hUart, NULL, 1);
        }
    } while (bUartBufferIsAvailable);

    return true;
}

/*!
 *  @brief  Function to get a string of chars from the UART until a newline char is encountered
 *
 *  Function to initialize, configure and open a given UART peripheral specified by the
 *  particular index value at a speed given by baudRate value. 8 bits datalength,
 *  no parity and 1 stop bit are fixed parameters
 *
 *  @pre    GetLine_UART() has been called
 *
 *  @param  index         Logical peripheral number for the UART indexed into
 *                        the UART_config table
 *
 *  @param  buf           pointer to unsigned char where the line will be stored
 *
 *  @return               The size of the Line (include the newline character
 *                        Size 0 means that the number of the string of chars has reached 255
 *
 */
uint8_t GetLine_UART(UART_Handle uart, unsigned char *buf) {
    uint8_t i = 0;
    unsigned char c;

    do {
        UART_read(uart,&c,1);
        buf[i++]=c;
        if (i==255) {
            i = 0;
            break;
        }
    } while (c !='\n');

    return i;
}

//*****************************************************************************
//
//! prints the formatted string on to the console
//!
//! \param[in]  format  - is a pointer to the character string specifying the
//!                       format in the following arguments need to be
//!                       interpreted.
//! \param[in]  [variable number of] arguments according to the format in the
//!             first parameters
//!
//! \return count of characters printed
//
//*****************************************************************************
int Report(const char *pcFormat,...)
{
    int iRet = 0;
    char        *pcBuff;
    char        *pcTemp;
    int iSize = 256;
    va_list list;

    pcBuff = (char*)malloc(iSize);
    if(pcBuff == NULL)
    {
        return(-1);
    }
    while(1)
    {
        va_start(list,pcFormat);
        iRet = vsnprintf(pcBuff, iSize, pcFormat, list);
        va_end(list);
        if((iRet > -1) && (iRet < iSize))
        {
            break;
        }
        else
        {
            iSize *= 2;
            if((pcTemp = realloc(pcBuff, iSize)) == NULL)
            {
                Message("Could not reallocate memory\n\r");
                iRet = -1;
                break;
            }
            else
            {
                pcBuff = pcTemp;
            }
        }
    }
    Message(pcBuff);
    free(pcBuff);

    return(iRet);
}
//*****************************************************************************
//
//! Outputs a character string to the console
//!
//! This function
//!        1. prints the input string character by character on to the console.
//!
//! \param[in]  str - is the pointer to the string to be printed
//!
//! \return none
//!
//! \note If UART_NONPOLLING defined in than Message or UART write should be
//!       called in task/thread context only.
//
//*****************************************************************************

void Message(const char *str)
{
#ifdef UART_NONPOLLING
    UART_write(uartHandle, str, strlen(str));
#else
    UART_writePolling(uartHandle, str, strlen(str));
#endif
}

//*****************************************************************************
//
//! Outputs a character to the console
//!
//! \param[in]  char    - A character to be printed
//!
//! \return none
//
//*****************************************************************************
void putch(char ch)
{
    UART_writePolling(uartHandle, &ch, 1);
}
