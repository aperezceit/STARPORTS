/*
 * hal_SPI.c
 *
 *  Created on: 13 abr. 2019
 *      Author: airizar
 */

#include <ti/drivers/SPI.h>
#include <stddef.h>
#include <stdint.h>

SPI_Handle Startup_SPI(uint_least8_t index, uint32_t dataRate) {

    SPI_Params spiParams;
    SPI_Handle spi;


    SPI_Params_init(&spiParams);
    spiParams.mode = SPI_MASTER;
    spiParams.transferMode = SPI_MODE_BLOCKING;
    spiParams.frameFormat = SPI_POL0_PHA1;
    spiParams.dataSize = 8;
    spiParams.bitRate = dataRate;
    SPI_init();
    spi = SPI_open(index, &spiParams);

    return spi;
}

uint8_t SPI_read_8bits(SPI_Handle spi, uint8_t Address, uint8_t *Data) {

    SPI_Transaction spiTransaction;
    uint8_t         TxBuffer[2];
    uint8_t         RxBuffer[2];

    bool            transferOK;

    TxBuffer[0] = (Address + 0x80);
    TxBuffer[1] = 0x00;

    spiTransaction.count = 2;
    spiTransaction.txBuf = (void *)TxBuffer;
    spiTransaction.rxBuf = (void *)RxBuffer;
    transferOK = SPI_transfer(spi, &spiTransaction);
    if (!transferOK) {
        // Error in SPI or transfer already in progress.
        return 1;
    } else {
        (*Data) = RxBuffer[1];
        return 0;
    }

}

uint8_t SPI_read_16bits(SPI_Handle spi, uint8_t Address, uint8_t *Data) {

    SPI_Transaction spiTransaction;
    uint16_t         TxBuffer[2];
    uint16_t         RxBuffer[2];

    bool            transferOK;

    TxBuffer[0] = (Address + 0x80)<<8;

    spiTransaction.count = 1;
    spiTransaction.txBuf = (void *)TxBuffer;
    spiTransaction.rxBuf = (void *)RxBuffer;
    transferOK = SPI_transfer(spi, &spiTransaction);
    if (!transferOK) {
        // Error in SPI or transfer already in progress.
        return 1;
    } else {
        (*Data) = RxBuffer[0] & 0x00FF;
        return 0;
    }

}

uint8_t SPI_write_8bits(SPI_Handle spi, uint8_t Address, uint8_t Data) {

    SPI_Transaction spiTransaction;
    uint8_t        TxBuffer[2];
    bool           transferOK;

    TxBuffer[0] = Address;
    TxBuffer[1] = Data;

    spiTransaction.count = 2;
    spiTransaction.txBuf = (void *)(TxBuffer);

    transferOK = SPI_transfer(spi, &spiTransaction);
    if (!transferOK) {
        // Error in SPI or transfer already in progress.
        return 1;
    } else {
        return 0;
    }

}

uint8_t SPI_write_16bits(SPI_Handle spi, uint8_t Address, uint8_t Data) {

    SPI_Transaction spiTransaction;
    uint16_t        TxBuffer[2];
    bool            transferOK;

    TxBuffer[0] = (Address << 8) + Data;

    spiTransaction.count = 1;
    spiTransaction.txBuf = (void *)(TxBuffer);

    transferOK = SPI_transfer(spi, &spiTransaction);
    if (!transferOK) {
        // Error in SPI or transfer already in progress.
        return 1;
    } else {
        return 0;
    }

}
