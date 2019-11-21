/*
 * hal_TMP006.c
 *
 *  Created on: 26 mar. 2019
 *      Author: airizar
 */

#include <stdint.h>
#include <stdbool.h>
#include <ti/drivers/I2C.h>

#include "hal_TMP006.h"


/*!
 *  @brief  Function to Set the TMP006 Sensor to Continuous Conversion at different rates
 *
 *
 *
 *  @pre    SetMOD_TMP006() has been called
 *
 *  @param  i2c       Handle to i2c device
 *
 *  @param  Slave     I2C Slave address of TMP006
 *
 *  @param  cr        Conversion rate in Conversions/minute
 *                    cr can only be one of {240, 120, 60, 30, 15}
 *
 *  @return 0 if success, 1 if error (cr has wrong value)
 *
 */
uint8_t SetMOD_TMP006(I2C_Handle i2c, uint_least8_t Slave, uint8_t cr) {

    uint8_t writeBuffer[3];
    I2C_Transaction i2cTransaction;
    bool status;

    writeBuffer[0] = 0x02;
    if (cr==240)
        writeBuffer[1] = 0x70;
    else if (cr==120)
        writeBuffer[1] = 0x72;
    else if (cr==60)
        writeBuffer[1] = 0x74;
    else if (cr==30)
        writeBuffer[1] = 0x76;
    else if (cr==15)
        writeBuffer[1] = 0x78;
    else
        return ERROR_WRONG_CR;

    writeBuffer[2] = 0x00;

    i2cTransaction.slaveAddress = Slave;
    i2cTransaction.writeBuf = writeBuffer;
    i2cTransaction.writeCount = 3;
    i2cTransaction.readBuf = NULL;
    i2cTransaction.readCount = 0;

    status = I2C_transfer(i2c, &i2cTransaction);

    return SUCCESS_CONF_TMP006;

}

int16_t GetTA_TMP006(I2C_Handle i2c, uint_least8_t Slave) {

    bool status;
    uint8_t writeBuffer;
    uint8_t readBuffer[2];
    I2C_Transaction i2cTransaction;
    int16_t Temp;

    writeBuffer = 0x01;

    i2cTransaction.slaveAddress = Slave;
    i2cTransaction.writeBuf = &writeBuffer;
    i2cTransaction.writeCount = 1;
    i2cTransaction.readBuf = readBuffer;
    i2cTransaction.readCount = 2;
    status = I2C_transfer(i2c, &i2cTransaction);

    Temp = (int16_t)( ((readBuffer[0]<<8) + readBuffer[1]) >> 2 );

    return Temp;
}

