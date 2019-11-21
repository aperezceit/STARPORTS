/*
 * hal_TMP006.h
 *
 *  Created on: 26 mar. 2019
 *      Author: airizar
 */

#ifndef HAL_TMP006_H_
#define HAL_TMP006_H_

#define ERROR_WRONG_CR      1
#define SUCCESS_CONF_TMP006 0

uint8_t SetMOD_TMP006(I2C_Handle i2c, uint_least8_t Slave, uint8_t cr);

int16_t GetTA_TMP006(I2C_Handle i2c, uint_least8_t Slave);


#endif /* HAL_TMP006_H_ */
