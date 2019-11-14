/*
 * hal_I2C.c
 *
 *  Created on: 25 mar. 2019
 *      Author: airizar
 */


#include <ti/drivers/I2C.h>
#include "Board.h"


I2C_Handle Startup_I2C(uint_least8_t index) {

    I2C_Params i2cParams;
    I2C_Handle i2c;

    I2C_init();
    I2C_Params_init(&i2cParams);

    i2cParams.transferMode = I2C_MODE_BLOCKING;
    i2cParams.bitRate = I2C_400kHz;

    i2c = I2C_open(index, &i2cParams);

    return i2c;
}

uint8_t I2C_read_8bits(I2C_Handle i2c, uint8_t Slave, uint8_t Address, uint8_t *RxBuffer, uint8_t nregs) {

    I2C_Transaction i2cTransaction;
    uint8_t         TxBuffer;
    bool            TransferOK;


    TxBuffer = Address;

    i2cTransaction.slaveAddress = Slave;
    i2cTransaction.writeBuf = &TxBuffer;
    i2cTransaction.writeCount = 1;
    i2cTransaction.readBuf = RxBuffer;
    i2cTransaction.readCount = nregs;

    TransferOK = I2C_transfer(i2c, &i2cTransaction);

    return (TransferOK? 0 : 1);
}


uint8_t I2C_write_8bits(I2C_Handle i2c, uint8_t Slave, uint8_t Address, uint8_t Val) {

    I2C_Transaction i2cTransaction;
    uint8_t         TxBuffer[2];
    bool            TransferOK;


    TxBuffer[0] = Address;
    TxBuffer[1] = Val;

    i2cTransaction.slaveAddress = Slave;
    i2cTransaction.writeBuf = TxBuffer;
    i2cTransaction.writeCount = 2;
    i2cTransaction.readBuf = NULL;
    i2cTransaction.readCount = 0;

    TransferOK = I2C_transfer(i2c, &i2cTransaction);

    return (TransferOK? 0 : 1);
}

