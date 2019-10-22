/*
 * hal_I2C.h
 *
 *  Created on: 25 mar. 2019
 *      Author: airizar
 */

#ifndef HAL_I2C_H_
#define HAL_I2C_H_

#include <ti/drivers/I2C.h>


I2C_Handle Startup_I2C(uint_least8_t index);

uint8_t I2C_read_8bits(I2C_Handle i2c, uint8_t Slave, uint8_t Address, uint8_t *RxBuffer, uint8_t nregs);

uint8_t I2C_write_8bits(I2C_Handle i2c, uint8_t Slave, uint8_t Address, uint8_t Val);


#endif /* HAL_I2C_H_ */
