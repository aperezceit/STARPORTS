/*
 * DS1374.c
 *
 *  Created on: 3 jun. 2019
 *      Author: airizar
 */


#include <ti/drivers/I2C.h>
#include "hal_I2C.h"
#include "DS1374.h"

uint32_t DS1374_Read_Tod(I2C_Handle i2c) {

    uint8_t RxBuffer[4];

    I2C_read_8bits(i2c, 0x68, TOD0, RxBuffer, 4);

    return RxBuffer[0] + (RxBuffer[1]<<8) + (RxBuffer[2]<<16) + (RxBuffer[3]<<24);

}
void DS1374_Write_Tod(I2C_Handle i2c, uint32_t Val) {

    I2C_write_8bits(i2c, 0x68, TOD3, (uint8_t)( (Val>>24) & 0xFF));
    I2C_write_8bits(i2c, 0x68, TOD2, (uint8_t)( (Val>>16) & 0xFF));
    I2C_write_8bits(i2c, 0x68, TOD1, (uint8_t)( (Val>>8) & 0xFF));
    I2C_write_8bits(i2c, 0x68, TOD0, (uint8_t)( Val & 0xFF));

}
uint32_t DS1374_Read_WdAlmb(I2C_Handle i2c) {

    uint8_t RxBuffer[3];

    I2C_read_8bits(i2c, 0x68, WDALMB0, RxBuffer, 3);

    return RxBuffer[0] + (RxBuffer[1]<<8) + (RxBuffer[2]<<16);
}

void DS1374_Write_WdAlmb(I2C_Handle i2c, uint32_t Val) {

    I2C_write_8bits(i2c, 0x68, WDALMB2, (uint8_t)( (Val>>16) & 0xFF));
    I2C_write_8bits(i2c, 0x68, WDALMB1, (uint8_t)( (Val>>8) & 0xFF));
    I2C_write_8bits(i2c, 0x68, WDALMB0, (uint8_t)( Val & 0xFF));

}

void DS1374_Write_Ctrl(I2C_Handle i2c) {

    uint8_t Val;

    Val = (1<<6) | (1<<3) | 1;

    I2C_write_8bits(i2c, 0x68, DS1374_CTRL, Val);

}
uint8_t DS1374_Read_Status(I2C_Handle i2c) {

    uint8_t RxBuffer;

    I2C_read_8bits(i2c, 0x68, DS1374_STATUS, &RxBuffer, 1);

    return RxBuffer;

}
void DS1374_Clear_AF(I2C_Handle i2c) {

    uint8_t RxBuffer;
    uint8_t Val;
    I2C_read_8bits(i2c, 0x68, DS1374_STATUS, &RxBuffer, 1);
    Val = RxBuffer & 0xFE;
    I2C_write_8bits(i2c, 0x68, DS1374_STATUS, Val);

}
