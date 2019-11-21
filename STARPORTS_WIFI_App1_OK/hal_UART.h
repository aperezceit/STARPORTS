/*
 * hal_UART.h
 *
 *  Created on: 25 mar. 2019
 *      Author: airizar
 */

#ifndef HAL_UART_H_
#define HAL_UART_H_

#include <ti/drivers/UART.h>
#include <stdbool.h>
#include "Board.h"


#define UART_PRINT Report
#define ERR_PRINT(x) Report("Error [%d] at line [%d] in function [%s]  \n\r",\
                                                                x, __LINE__, \
                                                                 __FUNCTION__)


UART_Handle Startup_UART(uint_least8_t index, uint32_t baudRate);

bool UART_resetInputBuffer(UART_Handle hUart);

uint8_t GetLine_UART(UART_Handle uart, unsigned char *buf);

int Report(const char *pcFormat,...);

void Message(const char *str);

void putch(char ch);

#endif /* HAL_UART_H_ */
