/*
 * hal_SPI.h
 *
 *  Created on: 13 abr. 2019
 *      Author: airizar
 */

#ifndef HAL_SPI_H_
#define HAL_SPI_H_

#include <ti/drivers/SPI.h>
#include <stddef.h>
#include <stdint.h>

SPI_Handle Startup_SPI(uint_least8_t index, uint32_t dataSize, uint32_t dataRate);

uint8_t SPI_write_8bits(SPI_Handle spi, uint8_t Address, uint8_t Data, uint8_t rnwMSB);

uint8_t SPI_read_8bits(SPI_Handle spi, uint8_t Address, uint8_t *RxBuffer, uint8_t nregs, uint8_t rnwMSB);

uint8_t SPI_write_16bits(SPI_Handle spi, uint8_t Address, uint8_t Data);

uint8_t SPI_read_16bits(SPI_Handle spi, uint8_t Address, uint8_t *Data);



#endif /* HAL_SPI_H_ */
