/*
 * ADXL355.h
 *
 *  Created on: 7 may. 2019
 *      Author: airizar
 */


#include <ti/drivers/SPI.h>
#include <stdbool.h>

#ifndef ADXL355_H_
#define ADXL355_H_

#define ADXL355__DEVICE_AD      0xAD     // contect of DEVID_AD (only-read) register
#define ADXL355_RESET          0x52     // reset code

typedef struct {
    // sensitivity
    float S[2][2];
    float St;
    // 0g offset
    float B[2][0];
} ADXL355_calibdata_t;

ADXL355_calibdata_t calib_data;
// gravity filed vectors
const static float t_sens = -9.05;
const static float t_bias = 1852;
float axis355_sens;

// -------------------------- //
// REGISTERS                  //
// -------------------------- //
typedef enum {
    DEVID_AD = 0x00,
    DEVID_MST = 0x01,
    PARTID = 0x02,
    REVID = 0x03,
    ADXL355_STATUS = 0x04,
    FIFO_ENTRIES = 0x05,
    TEMP2 = 0x06,
    TEMP1 = 0x07,
    XDATA3 = 0x08,
    XDATA2 = 0x09,
    XDATA1 = 0x0A,
    YDATA3 = 0x0B,
    YDATA2 = 0x0C,
    YDATA1 = 0x0D,
    ZDATA3 = 0x0E,
    ZDATA2 = 0x0F,
    ZDATA1 = 0x10,
    FIFO_DATA = 0x11,
    OFFSET_X_H = 0x1E,
    OFFSET_X_L = 0x1F,
    OFFSET_Y_H = 0x20,
    OFFSET_Y_L = 0x21,
    OFFSET_Z_H = 0x22,
    OFFSET_Z_L = 0x23,
    ACT_EN = 0x24,
    ACT_THRESH_H = 0x25,
    ACT_THRESH_L = 0x26,
    ACT_COUNT = 0x27,
    FILTER = 0x28,
    FIFO_SAMPLES = 0x29,
    INT_MAP = 0x2A,
    SYNC = 0x2B,
    RANGE = 0x2C,
    POWER_CTL = 0x2D,
    SELF_TEST = 0x2E,
    ADXL355_RESET_REG = 0x2F
} ADXL355_register_t;
// -------------------------- //
// REGISTERS - DEFAULT VALUES //
// -------------------------- //

// ADXL_STATUS
typedef enum {
    NVM_BUSY = 0x10,
    ACTIVITY = 0x08,
    FIFO_OVR = 0x04,
    FIFO_FULL = 0x02,
    DATA_RDY = 0x01
} ADXL355_status_t;

// Modes - POWER_CTL
typedef enum {
    DRDY_OFF = 0x04,
    TEMP_OFF = 0x02,
    STANDBY = 0x01,
    MEASUREMENT = 0x00
} ADXL355_modes_t;

// Activate Threshold - ACT_EN
typedef enum {
    ACT_Z = 0x04,
    ACT_Y = 0x02,
    ACT_X = 0x01
} ADXL355_act_ctl_t;

// High-Pass and Low-Pass Filter - FILTER
typedef enum {
    HPFOFF = 0x00,
    HPF247 = 0x10,
    HPF62 = 0x20,
    HPF15 = 0x30,
    HPF3 = 0x40,
    HPF09 = 0x50,
    HPF02 = 0x60,
    ODR4000HZ = 0x00,
    ODR2000HZ = 0x01,
    ODR1000HZ = 0x02,
    ODR500HZ = 0x03,
    ODR250HZ = 0x04,
    ODR125Hz = 0x05,
    ODR62HZ = 0x06,
    ODR31Hz = 0x07,
    ODR15Hz = 0x08,
    ODR7Hz = 0x09,
    ODR3HZ = 0x0A
} ADXL355_filter_ctl_t;

// External timing register - INT_MAP
typedef enum {
    OVR_EN = 0x04,
    FULL_EN = 0x02,
    RDY_EN = 0x01
} ADXL355_intmap_ctl_t;

// External timing register - SYNC
typedef enum {
    EXT_CLK = 0x04,
    INT_SYNC = 0x00,
    EXT_SYNC_NO_INT = 0x01,
    EXT_SYNC_INT = 0x02
} ADXL355_sync_ctl_t;

// polarity and range - RANGE

typedef enum {
    I2C_HIGH_SPEED = 0x80,
    I2C_FAST_MODE = 0x00
} ADXL355_i2c_speed_t;

typedef enum {
    INT_POL_LOW = 0x00,
    INT_POL_HIGH = 0x40
} ADXL355_int_pol_t;

typedef enum {
    RANGE2G = 0x01,
    RANGE4G = 0x02,
    RANGE8G = 0x03
} ADXL355_range_ctl_t;

// self test interrupt - INT
typedef enum {
    ST2 = 0x02,
    ST1 = 0x01
} ADXL355_int_ctl_t;


void ADXL355_Reset(SPI_Handle spi);
uint16_t ADXL355_DevId(SPI_Handle spi);
uint16_t ADXL355_PartId(SPI_Handle spi);
void ADXL355_Power_Conf(SPI_Handle spi, uint8_t Val);
void ADXL355_Range_Conf(SPI_Handle spi, uint8_t Val);
void ADXL355_Fifo_Samples(SPI_Handle spi, uint8_t Samples);
bool ADXL355_Fifo_Full(SPI_Handle spi);
bool ADXL355_Data_Rdy(SPI_Handle spi);
uint8_t ADXL355_Fifo_Entries(SPI_Handle spi);
void ADXL355_Filter_Conf(SPI_Handle spi, uint8_t Val);
int32_t ADXL355_XData(SPI_Handle spi);
int32_t ADXL355_YData(SPI_Handle spi);
int32_t ADXL355_ZData(SPI_Handle spi);
float ADXL355_Temp(SPI_Handle spi);
void ADXL355_Get_Accel_Frame(SPI_Handle spi, uint16_t Samples, int32_t *DataSensor);
void ADXL355_Fifo_Data(SPI_Handle spi, int32_t *Ax, int32_t *Ay, int32_t *Az, uint8_t Samples);
int32_t u20_to_s32(uint32_t uData);


#endif /* ADXL355_H_ */
