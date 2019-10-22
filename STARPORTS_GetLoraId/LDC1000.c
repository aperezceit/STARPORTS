/*
 * LDC1000.c
 *
 *  Created on: 2 jun. 2019
 *      Author: airizar
 */

#include "LDC1000.h"
#include <ti/drivers/SPI.h>
#include <math.h>
#include <STARPORTS_GetLoraId.h>
#include "hal_SPI.h"


uint8_t LDC1000_DevId(SPI_Handle spi) {

    uint8_t RxBuffer[2];

    SPI_read_8bits(spi,DEV_ID,RxBuffer,1,RNW_MSB);

    return RxBuffer[1];

}


void LDC1000_Write_Rp_Max(SPI_Handle spi, uint8_t Val) {

    SPI_write_8bits(spi,RP_MAX,Val,RNW_MSB);

}

void LDC1000_Write_Rp_Min(SPI_Handle spi, uint8_t Val) {

    SPI_write_8bits(spi,RP_MIN,Val,RNW_MSB);

}


void LDC1000_Write_Min_Freq(SPI_Handle spi, uint8_t Val) {

    SPI_write_8bits(spi,MIN_FREQ,Val,RNW_MSB);

}

void LDC1000_Write_Conf(SPI_Handle spi, uint8_t Val) {

    SPI_write_8bits(spi,LDC_CONFIG,Val,RNW_MSB);

}

void LDC1000_Write_Intb_Conf(SPI_Handle spi, uint8_t Val) {

    SPI_write_8bits(spi,INTB_CONFIG,Val,RNW_MSB);

}

void LDC1000_Write_Pow_Conf(SPI_Handle spi, uint8_t Val) {

    SPI_write_8bits(spi,POW_CONFIG,Val,RNW_MSB);

}

void LDC1000_Write_Clk_Conf(SPI_Handle spi, uint8_t Val) {

    SPI_write_8bits(spi,CLK_CONFIG,Val,RNW_MSB);

}

uint8_t LDC1000_Read_Status(SPI_Handle spi) {

    uint8_t RxBuffer[2];

    SPI_read_8bits(spi,LDC_STATUS,RxBuffer,1,RNW_MSB);

    return RxBuffer[1];

}


uint16_t LDC1000_Read_Proximity(SPI_Handle spi) {

    uint8_t RxBuffer[3];

    SPI_read_8bits(spi,PROX_LSB,RxBuffer,2,RNW_MSB);

    return ( RxBuffer[1] + (RxBuffer[2]<<8) );

}

void LDC1000_Get_Proximity_Frame(SPI_Handle spi, uint16_t Samples, int32_t *DataSensor) {

    int i=0;
    uint32_t Proximity[512];
    uint32_t ProxMean, RpMean, RpRms;
    double ProxRms;

    ProxMean = 0;
    while(i<Samples) {
        while ( LDC1000_Read_Status(spi) & DRDYB_MASK ) {;}
        Proximity[i] = (uint32_t)LDC1000_Read_Proximity(spi);
        ProxMean += Proximity[i];
        i++;
    }
    ProxMean = ProxMean/Samples;

    ProxRms = 0.0;
    for (i=0;i<Samples;i++) {
        ProxRms += ( (Proximity[i]-ProxMean)*(Proximity[i]-ProxMean) );
    }

    RpMean = (uint32_t)(RpCalc(ProxMean, 981.748, 436.332) * 10000 );
    RpRms = (uint32_t)(RpCalc((uint32_t)sqrt(ProxRms/Samples), 981.748, 436.332) * 10000 );

    DataSensor[0] = (int32_t)ProxMean;
    DataSensor[1] = (int32_t)sqrt(ProxRms/Samples);

    // DataSensor[0] = (int32_t)RpMean;
    // DataSensor[1] = (int32_t)RpRms;

}

uint32_t LDC1000_Read_Fcount(SPI_Handle spi) {

    uint8_t RxBuffer[4];

    SPI_read_8bits(spi,FCOUNT_LSB,RxBuffer,3,RNW_MSB);

    return ( RxBuffer[1] + (RxBuffer[2]<<8) + (RxBuffer[3]<<16) );

}

float RpCalc(uint32_t Proximity, float RpMax, float RpMin) {

    float Y = (1<<15)/Proximity;
    float Rp;


    Rp = RpMax * RpMin / (RpMin*(1-Y) + RpMax*Y);

    return Rp;

}
