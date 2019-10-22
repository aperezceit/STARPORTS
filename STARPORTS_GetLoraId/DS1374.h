/*
 * DS1374.h
 *
 *  Created on: 3 jun. 2019
 *      Author: airizar
 */

#ifndef DS1374_H_
#define DS1374_H_

#include <ti/drivers/I2C.h>

typedef enum {
    TOD0 = 0x00,
    TOD1 = 0x01,
    TOD2 = 0x02,
    TOD3 = 0x03,
    WDALMB0 = 0x04,
    WDALMB1 = 0x05,
    WDALMB2 = 0x06,
    DS1374_CTRL = 0x07,
    DS1374_STATUS = 0x08,
    DS1374_TC = 0x09
} DS1374_register_t;

uint32_t DS1374_Read_Tod(I2C_Handle i2c);
void DS1374_Write_Tod(I2C_Handle i2c, uint32_t Val);
uint32_t DS1374_Read_WdAlmb(I2C_Handle i2c);
void DS1374_Write_WdAlmb(I2C_Handle i2c, uint32_t Val);
void DS1374_Write_Ctrl(I2C_Handle i2c);
uint8_t DS1374_Read_Status(I2C_Handle i2c);
void DS1374_Clear_AF(I2C_Handle i2c);


#endif /* DS1374_H_ */
