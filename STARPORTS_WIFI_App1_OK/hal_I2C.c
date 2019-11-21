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
    if (i2c == NULL) {
        while (1);  // SPI_open() failed
    }

    return i2c;
}

