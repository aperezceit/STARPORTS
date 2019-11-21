/*
 * BME280.c
 *
 *  Created on: 29 may. 2019
 *      Author: airizar
 */
//gituhub test
#include "STARPORTS_App.h"
#include "BME280.h"
#include "hal_SPI.h"
#include "hal_I2C.h"
#include <ti/drivers/SPI.h>
#include <ti/drivers/I2C.h>
#include <stdbool.h>
#include <math.h>
#include <stdlib.h>

void BME280_Reset(SPI_Handle spi) {

    SPI_write_8bits(spi, BME280_RESET_REG, BME280_RESET, RNW_MSB);

}

uint8_t BME280_DevId(SPI_Handle spi) {

    uint8_t RxBuffer[2];

    SPI_read_8bits(spi, DEVID, RxBuffer, 1, RNW_MSB);
    return RxBuffer[1];

}

void BME280_Write_Ctrl_Hum(SPI_Handle spi, uint8_t Val) {


    SPI_write_8bits(spi, CTRL_HUM, Val, RNW_MSB);

}


uint8_t BME280_Read_Ctrl_Hum(SPI_Handle spi) {

    uint8_t RxBuffer[2];

    SPI_read_8bits(spi, CTRL_HUM, RxBuffer, 1, RNW_MSB);
    return RxBuffer[1];

}

void BME280_Write_Ctrl_Meas(SPI_Handle spi, uint8_t Val) {

    SPI_write_8bits(spi, CTRL_MEAS, Val, RNW_MSB);

}

uint8_t BME280_Read_Ctrl_Meas(SPI_Handle spi) {

    uint8_t RxBuffer[2];

    SPI_read_8bits(spi, CTRL_MEAS, RxBuffer, 1, RNW_MSB);

    return RxBuffer[1];

}
void BME280_Write_Config(SPI_Handle spi, uint8_t Val) {

    SPI_write_8bits(spi, CONFIG, Val, RNW_MSB);

}
uint8_t BME280_Read_Config(SPI_Handle spi) {

    uint8_t RxBuffer[2];

    SPI_read_8bits(spi, CONFIG, RxBuffer, 1, RNW_MSB);

    return RxBuffer[1];

}
uint8_t BME280_Read_Status(SPI_Handle spi) {

    uint8_t RxBuffer[2];

    SPI_read_8bits(spi, BME280_STATUS, RxBuffer, 1, RNW_MSB);

    return RxBuffer[1];

}

void BME280_Read_Calib(SPI_Handle spi, struct bme280_calib_data *MyCalib) {

    uint8_t RxBuffer00[CALIB00_NDATA+1];
    uint8_t RxBuffer26[CALIB26_NDATA+1];

    SPI_read_8bits(spi, CALIB00, RxBuffer00, CALIB00_NDATA, RNW_MSB);
    SPI_read_8bits(spi, CALIB26, RxBuffer26, CALIB26_NDATA, RNW_MSB);

    MyCalib->dig_T1 = (RxBuffer00[1] + (RxBuffer00[2]<<8));
    MyCalib->dig_T2 = (RxBuffer00[3] + (RxBuffer00[4]<<8));
    MyCalib->dig_T3 = (RxBuffer00[5] + (RxBuffer00[6]<<8));


    MyCalib->dig_P1 = (RxBuffer00[7] + (RxBuffer00[8]<<8));
    MyCalib->dig_P2 = (RxBuffer00[9] + (RxBuffer00[10]<<8));
    MyCalib->dig_P3 = (RxBuffer00[11] + (RxBuffer00[12]<<8));
    MyCalib->dig_P4 = (RxBuffer00[13] + (RxBuffer00[14]<<8));
    MyCalib->dig_P5 = (RxBuffer00[15] + (RxBuffer00[16]<<8));
    MyCalib->dig_P6 = (RxBuffer00[17] + (RxBuffer00[18]<<8));
    MyCalib->dig_P7 = (RxBuffer00[19] + (RxBuffer00[20]<<8));
    MyCalib->dig_P8 = (RxBuffer00[21] + (RxBuffer00[22]<<8));
    MyCalib->dig_P9 = (RxBuffer00[23] + (RxBuffer00[24]<<8));

    MyCalib->dig_H1 = RxBuffer00[26];
    MyCalib->dig_H2 = RxBuffer26[1] + (RxBuffer26[2]<<8);
    MyCalib->dig_H3 = RxBuffer26[3];
    MyCalib->dig_H4 = (RxBuffer26[4]<<4) + (RxBuffer26[5] & 0x0F);
    MyCalib->dig_H5 = ((RxBuffer26[5]>>4) & 0x0F) + (RxBuffer26[6]<<4);
    MyCalib->dig_H6 = RxBuffer26[7];

}


uint32_t BME280_Read_Pressure(SPI_Handle spi) {

    uint8_t RxBuffer[4];
    uint32_t Pressure;
    SPI_read_8bits(spi, PRESS_MSB, RxBuffer, 3, RNW_MSB);

    Pressure = ((RxBuffer[1]<<12) + (RxBuffer[2]<<4) + (RxBuffer[3]>>4));

    return Pressure;

}

uint16_t BME280_Read_Humidity(SPI_Handle spi) {

    uint8_t RxBuffer[3];
    uint16_t Humidity;
    SPI_read_8bits(spi, HUM_MSB, RxBuffer, 2, RNW_MSB);

    Humidity = (RxBuffer[1]<<8) + RxBuffer[2];

    return Humidity;

}

uint32_t BME280_Read_Temperature(SPI_Handle spi) {

    uint8_t RxBuffer[4];
    uint32_t Temperature;
    SPI_read_8bits(spi, TEMP_MSB, RxBuffer, 3, RNW_MSB);

    Temperature = ((RxBuffer[1]<<12) + (RxBuffer[2]<<4) + (RxBuffer[3]>>4));

    return Temperature;

}

/*!
 *  @brief  Function to compensate the value of the humidity read from the BME280 registers. Function compensate_temperature() has to be called before this function
 *
 *  @pre    compensate_humidity() has been called
 *
 *  @param  HumidityUncomp          Uncompensated value of Humidity as read from the BME280 registers
 *
 *  @param  bme280_calib_data       Pointer to an struct that holds the calibration values of the sensor
 *
 *  @return uint32_t                Returns the compensated humidity value in units 1024 * %relative humidity
 *
 */
uint32_t compensate_humidity(uint16_t HumidityUncomp, struct bme280_calib_data *MyCalib) {
    int32_t var1;
    int32_t var2;
    int32_t var3;
    int32_t var4;
    int32_t var5;
    uint32_t humidity;
    uint32_t humidity_max = 102400;

    var1 = MyCalib->t_fine - ((int32_t)76800);
    var2 = (int32_t)(HumidityUncomp * 16384);
    var3 = (int32_t)(((int32_t)MyCalib->dig_H4) * 1048576);
    var4 = ((int32_t)MyCalib->dig_H5) * var1;
    var5 = (((var2 - var3) - var4) + (int32_t)16384) / 32768;
    var2 = (var1 * ((int32_t)MyCalib->dig_H6)) / 1024;
    var3 = (var1 * ((int32_t)MyCalib->dig_H3)) / 2048;
    var4 = ((var2 * (var3 + (int32_t)32768)) / 1024) + (int32_t)2097152;
    var2 = ((var4 * ((int32_t)MyCalib->dig_H2)) + 8192) / 16384;
    var3 = var5 * var2;
    var4 = ((var3 / 32768) * (var3 / 32768)) / 128;
    var5 = var3 - ((var4 * ((int32_t)MyCalib->dig_H1)) / 16);
    var5 = (var5 < 0 ? 0 : var5);
    var5 = (var5 > 419430400 ? 419430400 : var5);
    humidity = (uint32_t)(var5 / 4096);
    if (humidity > humidity_max)
    {
        humidity = humidity_max;
    }

    return humidity;
}

/*!
 *  @brief  Function to compensate the value of the temperature read from the BME280 registers.
 *
 *  @pre    compensate_temperature() has been called
 *
 *  @param  TemperatureUncomp          Uncompensated value of Temperature as read from the BME280 registers
 *
 *  @param  bme280_calib_data       Pointer to an struct that holds the calibration values of the sensor
 *
 *  @return int32_t                Returns the compensated temperature value in units 100 * ºC
 *
 */
int32_t compensate_temperature(uint32_t TemperatureUncomp, struct bme280_calib_data *MyCalib) {
    int32_t var1;
    int32_t var2;
    int32_t temperature;
    int32_t temperature_min = -4000;
    int32_t temperature_max = 8500;

    var1 = (int32_t)((TemperatureUncomp / 8) - ((int32_t)MyCalib->dig_T1 * 2));
    var1 = (var1 * ((int32_t)MyCalib->dig_T2)) / 2048;
    var2 = (int32_t)((TemperatureUncomp / 16) - ((int32_t)MyCalib->dig_T1));
    var2 = (((var2 * var2) / 4096) * ((int32_t)MyCalib->dig_T3)) / 16384;
    MyCalib->t_fine = var1 + var2;
    temperature = (MyCalib->t_fine * 5 + 128) / 256;
    if (temperature < temperature_min)
    {
        temperature = temperature_min;
    }
    else if (temperature > temperature_max)
    {
        temperature = temperature_max;
    }

    return temperature;
}

/*!
 *  @brief  Function to compensate the value of the pressure read from the BME280 registers.
 *
 *  @pre    compensate_pressure() has been called
 *
 *  @param  PressureUncomp          Uncompensated value of Pressure as read from the BME280 registers
 *
 *  @param  bme280_calib_data       Pointer to an struct that holds the calibration values of the sensor
 *
 *  @return uint32_t                Returns the compensated pressure value in units of Pascals
 *
 */
uint32_t compensate_pressure(uint32_t PressureUncomp, struct bme280_calib_data *MyCalib)
{
    int32_t var1;
    int32_t var2;
    int32_t var3;
    int32_t var4;
    uint32_t var5;
    uint32_t pressure;
    uint32_t pressure_min = 30000;
    uint32_t pressure_max = 110000;

    var1 = (((int32_t)MyCalib->t_fine) / 2) - (int32_t)64000;
    var2 = (((var1 / 4) * (var1 / 4)) / 2048) * ((int32_t)MyCalib->dig_P6);
    var2 = var2 + ((var1 * ((int32_t)MyCalib->dig_P5)) * 2);
    var2 = (var2 / 4) + (((int32_t)MyCalib->dig_P4) * 65536);
    var3 = (MyCalib->dig_P3 * (((var1 / 4) * (var1 / 4)) / 8192)) / 8;
    var4 = (((int32_t)MyCalib->dig_P2) * var1) / 2;
    var1 = (var3 + var4) / 262144;
    var1 = (((32768 + var1)) * ((int32_t)MyCalib->dig_P1)) / 32768;

    /* avoid exception caused by division by zero */
    if (var1)
    {
        var5 = (uint32_t)((uint32_t)1048576) - PressureUncomp;
        pressure = ((uint32_t)(var5 - (uint32_t)(var2 / 4096))) * 3125;
        if (pressure < 0x80000000)
        {
            pressure = (pressure << 1) / ((uint32_t)var1);
        }
        else
        {
            pressure = (pressure / (uint32_t)var1) * 2;
        }
        var1 = (((int32_t)MyCalib->dig_P9) * ((int32_t)(((pressure / 8) * (pressure / 8)) / 8192))) / 4096;
        var2 = (((int32_t)(pressure / 4)) * ((int32_t)MyCalib->dig_P8)) / 8192;
        pressure = (uint32_t)((int32_t)pressure + ((var1 + var2 + MyCalib->dig_P7) / 16));
        if (pressure < pressure_min)
        {
            pressure = pressure_min;
        }
        else if (pressure > pressure_max)
        {
            pressure = pressure_max;
        }
    }
    else
    {
        pressure = pressure_min;
    }

    return pressure;
}
