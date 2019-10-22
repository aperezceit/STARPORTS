/*
 * BME280.h
 *
 *  Created on: 7 may. 2019
 *      Author: airizar
 */


#include <ti/drivers/SPI.h>
#include <ti/drivers/I2C.h>
#include <stdbool.h>

#ifndef BME280_H_
#define BME280_H_

#define BME280_DEVICE_AD      0x60     // contect of DEVID_AD (only-read) register
#define BME280_RESET          0xB6     // reset code

#define CALIB00_NDATA    26
#define CALIB26_NDATA    16

// -------------------------- //
// REGISTERS                  //
// -------------------------- //
typedef enum {
    HUM_LSB = 0xFE,
    HUM_MSB = 0xFD,
    TEMP_XLSB = 0xFC,
    TEMP_LSB = 0xFB,
    TEMP_MSB = 0xFA,
    PRESS_XLSB = 0xF9,
    PRESS_LSB = 0xF8,
    PRESS_MSB = 0xF7,
    CONFIG = 0xF5,
    CTRL_MEAS = 0xF4,
    BME280_STATUS = 0xF3,
    CTRL_HUM = 0xF2,
    CALIB26 = 0xE1,
    BME280_RESET_REG = 0xE0,
    DEVID = 0xD0,
    CALIB00 = 0x88
} BME280_register_t;

// -------------------------- //
// REGISTERS - DEFAULT VALUES //
// -------------------------- //
// CTRL_MEAS
typedef enum {
    OSRS_TX1 = (0x01 << 5) & 0xE0,
    OSRS_TX2 = (0x02 << 5) & 0xE0,
    OSRS_TX4 = (0x03 << 5) & 0xE0,
    OSRS_TX8 = (0x04 << 5) & 0xE0,
    OSRS_TX16 = (0x05 << 5) & 0xE0
} BME280_osrs_t_t;

typedef enum {
    OSRS_PX1 = (0x01 << 2) & 0x1C,
    OSRS_PX2 = (0x02 << 2) & 0x1C,
    OSRS_PX4 = (0x03 << 2) & 0x1C,
    OSRS_PX8 = (0x04 << 2) & 0x1C,
    OSRS_PX16 = (0x05 << 2) & 0x1C
} BME280_osrs_p_t;

typedef enum {
    SLEEP = 0x00,
    FORCED = 0x01,
    NORMAL = 0x03
} BME280_modes_t;

// CTRL_HUM
typedef enum {
    OSRS_HX1 = 0x01,
    OSRS_HX2 = 0x02,
    OSRS_HX4 = 0x03,
    OSRS_HX8 = 0x04,
    OSRS_HX16 = 0x05
} BME280_osrs_h_t;

// STATUS
typedef enum {
    MEASURING = 0x08,
    UPDATE = 0x01
} BME280_status_t;


// CONFIG
typedef enum {
    T_SB_0p5 = (0x00 << 5) & 0xE0,
    T_SB_62p5 = (0x01 << 5) & 0xE0,
    T_SB_125 = (0x02 << 5) & 0xE0,
    T_SB_250 = (0x03 << 5) & 0xE0,
    T_SB_500 = (0x04 << 5) & 0xE0,
    T_SB_1000 = (0x05 << 5) & 0xE0,
    T_SB_10 = (0x06 << 5) & 0xE0,
    T_SB_20 = (0x07 << 5) & 0xE0
} BME280_t_sb_t;

typedef enum {
    FILTER_OFF = (0x00 << 2) & 0x1C,
    FILTER_2 = (0x01 << 2) & 0x1C,
    FILTER_4 = (0x02 << 2) & 0x1C,
    FILTER_8 = (0x03 << 2) & 0x1C,
    FILTER_16 = (0x04 << 2) & 0x1C
} BME280_filter_t;

typedef enum {
    SPI3W_ON = 0x01,
    SPI3W_OFF = 0x00
} BME280_spi3w_t;



struct bme280_calib_data
{
    uint16_t dig_T1;
    int16_t dig_T2;
    int16_t dig_T3;
    uint16_t dig_P1;
    int16_t dig_P2;
    int16_t dig_P3;
    int16_t dig_P4;
    int16_t dig_P5;
    int16_t dig_P6;
    int16_t dig_P7;
    int16_t dig_P8;
    int16_t dig_P9;
    uint8_t dig_H1;
    int16_t dig_H2;
    uint8_t dig_H3;
    int16_t dig_H4;
    int16_t dig_H5;
    int8_t dig_H6;
    int32_t t_fine;
};

void BME280_Reset(SPI_Handle spi);
uint8_t BME280_DevId(SPI_Handle spi);
void BME280_Write_Ctrl_Hum(SPI_Handle spi, uint8_t Val);
uint8_t BME280_Read_Ctrl_Hum(SPI_Handle spi);
void BME280_Write_Ctrl_Meas(SPI_Handle spi, uint8_t Val);
uint8_t BME280_Read_Ctrl_Meas(SPI_Handle spi);
void BME280_Write_Config(SPI_Handle spi, uint8_t Val);
uint8_t BME280_Read_Config(SPI_Handle spi);
uint8_t BME280_Read_Status(SPI_Handle spi);
void BME280_Read_Calib(SPI_Handle spi, struct bme280_calib_data *MyCalib);
uint32_t BME280_Read_Pressure(SPI_Handle spi);
uint16_t BME280_Read_Humidity(SPI_Handle spi);
uint32_t BME280_Read_Temperature(SPI_Handle spi);
uint32_t compensate_humidity(uint16_t HumidityUncomp, struct bme280_calib_data *MyCalib);
int32_t compensate_temperature(uint32_t TemperatureUncomp, struct bme280_calib_data *MyCalib);
uint32_t compensate_pressure(uint32_t PressureUncomp, struct bme280_calib_data *MyCalib);


#endif /* BME280_H_ */
