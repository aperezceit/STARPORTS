/*
 * ADXL355.c
 *
 *  Created on: 7 may. 2019
 *      Author: airizar
 *////


#include "STARPORTS_App.h"
#include "ADXL355.h"
#include "hal_SPI.h"
#include <ti/drivers/SPI.h>
#include <stdbool.h>
#include <math.h>
#include <stdlib.h>


uint16_t ADXL355_DevId(SPI_Handle spi) {

    uint8_t RxBuffer[3];
    uint16_t DevId;

    SPI_read_8bits(spi, DEVID_AD, RxBuffer, 2, RNW_LSB);

    DevId = (RxBuffer[1]<<8) + RxBuffer[2];

    return DevId;

}

uint16_t ADXL355_PartId(SPI_Handle spi) {

    uint8_t RxBuffer[3];
    uint16_t PartId;

    SPI_read_8bits(spi, PARTID, RxBuffer, 2, RNW_LSB);

    PartId = (RxBuffer[1]<<8) + RxBuffer[2];

    return PartId;

}

void ADXL355_Reset(SPI_Handle spi) {

    SPI_write_8bits(spi, ADXL355_RESET_REG, ADXL355_RESET, RNW_LSB);

}

void ADXL355_Power_Conf(SPI_Handle spi, uint8_t Val) {

    SPI_write_8bits(spi, POWER_CTL, Val, RNW_LSB);

}

void ADXL355_Range_Conf(SPI_Handle spi, uint8_t Val) {

    SPI_write_8bits(spi, RANGE, Val, RNW_LSB);

}

void ADXL355_Fifo_Samples(SPI_Handle spi, uint8_t Samples) {

    SPI_write_8bits(spi, FIFO_SAMPLES, Samples, RNW_LSB);
}

uint8_t ADXL355_Fifo_Entries(SPI_Handle spi) {

    uint8_t RxBuffer[2];

    SPI_read_8bits(spi, FIFO_ENTRIES, RxBuffer, 1, RNW_LSB);

    return (RxBuffer[1] & 0x7F);
}

void ADXL355_Filter_Conf(SPI_Handle spi, uint8_t Val) {

    SPI_write_8bits(spi, FILTER, Val, RNW_LSB);

}

int32_t ADXL355_XData(SPI_Handle spi) {

    uint8_t RxData[4];
    uint32_t uData;

    SPI_read_8bits(spi, XDATA3, RxData, 3, RNW_LSB);
    uData = (int32_t)(RxData[1] << 12) + (int32_t)(RxData[2] << 4) + (int32_t)(RxData[3] >> 4);

    return u20_to_s32(uData);

}

int32_t ADXL355_YData(SPI_Handle spi) {

    uint8_t RxData[4];
    uint32_t uData;

    SPI_read_8bits(spi, YDATA3, RxData, 3, RNW_LSB);
    uData = (int32_t)(RxData[1] << 12) + (int32_t)(RxData[2] << 4) + (int32_t)(RxData[3] >> 4);

    return u20_to_s32(uData);

}

int32_t ADXL355_ZData(SPI_Handle spi) {

    uint8_t RxData[4];
    uint32_t uData;

    SPI_read_8bits(spi, ZDATA3, RxData, 3, RNW_LSB);
    uData = (int32_t)(RxData[1] << 12) + (int32_t)(RxData[2] << 4) + (int32_t)(RxData[3] >> 4);

    return u20_to_s32(uData);
}

float ADXL355_Temp(SPI_Handle spi) {

    uint8_t RxData[3];
    uint16_t uData;

    SPI_read_8bits(spi, TEMP2, RxData, 2, RNW_LSB);
    uData = ((RxData[1] & 0x0F) << 8) + RxData[2];

    return (uData-t_bias)/t_sens + 25.0;

}

bool ADXL355_Fifo_Full(SPI_Handle spi) {

    uint8_t RxData[2];
    bool Fifo_Full;

    SPI_read_8bits(spi, ADXL355_STATUS, RxData, 1, RNW_LSB);

    Fifo_Full = (RxData[1] & 0x02) >> 1;

    return Fifo_Full;
}

bool ADXL355_Data_Rdy(SPI_Handle spi) {

    uint8_t RxData[2];
    bool Data_Rdy;

    SPI_read_8bits(spi, ADXL355_STATUS, RxData, 1, RNW_LSB);

    Data_Rdy = (RxData[1] & DATA_RDY);

    return Data_Rdy;
}


void ADXL355_Get_Accel_Frame(SPI_Handle spi, uint16_t Samples, int32_t *DataSensor)  {

    int i=0;
    int32_t Ax[512], Ay[512], Az[512];
    int32_t xMean, yMean, zMean;
    double xRms, yRms, zRms;

    xMean=0;
    yMean=0;
    zMean=0;
    while (i<Samples) {
        while(!ADXL355_Data_Rdy(spi)) {}
        Ax[i] = ADXL355_XData(spi);
        Ay[i] = ADXL355_YData(spi);
        Az[i] = ADXL355_ZData(spi);
        xMean += Ax[i];
        yMean += Ay[i];
        zMean += Az[i];
        i++;
    }

    xMean = xMean / Samples;
    yMean = yMean / Samples;
    zMean = zMean / Samples;

    xRms = 0.0;
    yRms = 0.0;
    zRms = 0.0;
    for (i=0;i<Samples;i++) {
        xRms += ( (Ax[i]-xMean)*(Ax[i]-xMean) );
        yRms += ( (Ay[i]-yMean)*(Ay[i]-yMean) );
        zRms += ( (Az[i]-zMean)*(Az[i]-zMean) );
    }

    DataSensor[0] = xMean;
    DataSensor[1] = yMean;
    DataSensor[2] = zMean;
    DataSensor[3] = (int32_t)sqrt(xRms/Samples);
    DataSensor[4] = (int32_t)sqrt(yRms/Samples);
    DataSensor[5] = (int32_t)sqrt(zRms/Samples);

}

/*
void ADXL_Get_Accel_Frame(SPI_Handle spi, struct ADXL355_Data *MyADXL355)  {

    int i=0;
    int32_t Ax[512], Ay[512], Az[512];
    int32_t xMean, yMean, zMean;
    double xRms, yRms, zRms;

    xMean=0;
    yMean=0;
    zMean=0;
    while (i<(MyADXL355->NSamples)) {
        while(!ADXL_Data_Rdy(spi)) {}
        Ax[i] = ADXL_XData(spi);
        Ay[i] = ADXL_YData(spi);
        Az[i] = ADXL_ZData(spi);
        xMean += Ax[i];
        yMean += Ay[i];
        zMean += Az[i];
        i++;
    }

    xMean = xMean / (MyADXL355->NSamples);
    yMean = yMean / (MyADXL355->NSamples);
    zMean = zMean / (MyADXL355->NSamples);

    xRms = 0.0;
    yRms = 0.0;
    zRms = 0.0;
    for (i=0;i<(MyADXL355->NSamples);i++) {
        xRms += ( (Ax[i]-xMean)*(Ax[i]-xMean) );
        yRms += ( (Ay[i]-yMean)*(Ay[i]-yMean) );
        zRms += ( (Az[i]-zMean)*(Az[i]-zMean) );
    }

    MyADXL355->AxMean = xMean;
    MyADXL355->AyMean = yMean;
    MyADXL355->AzMean = zMean;
    MyADXL355->AxRms = (int32_t)sqrt(xRms/(MyADXL355->NSamples));
    MyADXL355->AyRms = (int32_t)sqrt(yRms/(MyADXL355->NSamples));
    MyADXL355->AzRms = (int32_t)sqrt(zRms/(MyADXL355->NSamples));

}
*/


void ADXL355_Fifo_Data(SPI_Handle spi, int32_t *Ax, int32_t *Ay, int32_t *Az, uint8_t Samples) {

    uint8_t RxData[10];
    int8_t i;
    uint32_t uAx, uAy, uAz;

    // Polling the FIFO_FULL Register of STATUS reg
    while (!ADXL355_Fifo_Full(spi)) {}

    i=0;
    while (i<Samples) {
        SPI_read_8bits(spi, FIFO_DATA, RxData, 9, RNW_LSB);
        uAx = (RxData[1] << 12) + (RxData[2] << 4) + (RxData[3] >> 4);
        uAy = (RxData[4] << 12) + (RxData[5] << 4) + (RxData[6] >> 4);
        uAz = (RxData[7] << 12) + (RxData[8] << 4) + (RxData[9] >> 4);
        Ax[i] = u20_to_s32(uAx);
        Ay[i] = u20_to_s32(uAy);
        Az[i] = u20_to_s32(uAz);
        i++;
    }

}

int32_t u20_to_s32(uint32_t uData) {

    int32_t iData;

    if (uData & 0x80000) {
        iData = uData | 0xFFF00000;
    } else {
        iData = (int32_t)uData;
    }

    return iData;

}
