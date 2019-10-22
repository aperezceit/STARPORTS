/*
 * LDC1000.h
 *
 *  Created on: 7 may. 2019
 *      Author: airizar
 */


#include <ti/drivers/SPI.h>
#include <ti/drivers/I2C.h>
#include <stdbool.h>

#ifndef LDC1000_H_
#define LDC1000_H_

#define LDC1000_DEVICE_AD      0x84     // content of DEVID_AD (only-read) register


// -------------------------- //
// REGISTERS                  //
// -------------------------- //
typedef enum {
    DEV_ID = 0x00,
    RP_MAX = 0x01,
    RP_MIN = 0x02,
    MIN_FREQ = 0x03,
    LDC_CONFIG = 0x04,
    CLK_CONFIG = 0x05,
    THR_HIGH_LSB = 0x06,
    THR_HIGH_MSB = 0x07,
    THR_LOW_LSB = 0x08,
    THR_LOW_MSB = 0x09,
    INTB_CONFIG = 0x0A,
    POW_CONFIG = 0x0B,
    LDC_STATUS = 0x20,
    PROX_LSB = 0x21,
    PROX_MSB = 0x22,
    FCOUNT_LSB = 0x23,
    FCOUNT_XSB = 0x24,
    FCOUNT_MSB = 0x25
} LDC1000_register_t;

// -------------------------- //
// REGISTERS - DEFAULT VALUES //
// -------------------------- //
// RP_MAX
typedef enum {
    RPMAX3936 = 0x00,
    RPMAX3141 = 0x01,
    RPMAX2243 = 0x02,
    RPMAX1745 = 0x03,
    RPMAX1308 = 0x04,
    RPMAX0981 = 0x05,
    RPMAX0747 = 0x06,
    RPMAX0581 = 0x07,
    RPMAX0436 = 0x08,
    RPMAX0349 = 0x09,
    RPMAX0249 = 0x0a,
    RPMAX0193 = 0x0b,
    RPMAX0145 = 0x0c,
    RPMAX0109 = 0x0d,
    RPMAX0083 = 0x0e,
    RPMAX0064 = 0x0f,
    RPMAX0048 = 0x10,
    RPMAX0038 = 0x11,
    RPMAX0027 = 0x12,
    RPMAX0021 = 0x13,
    RPMAX0016 = 0x14,
    RPMAX0012 = 0x15,
    RPMAX0009 = 0x16,
    RPMAX0007 = 0x17,
    RPMAX0005 = 0x18,
    RPMAX0004 = 0x19,
    RPMAX0003 = 0x1a,
    RPMAX0002 = 0x1b,
    RPMAX0001p7 = 0x1c,
    RPMAX0001p3 = 0x1d,
    RPMAX0001p0 = 0x1e,
    RPMAX0000 = 0x1f
} LDC1000_rp_max_t;

// RP_MIN
typedef enum {
    RPMIN3926 = 0x20,
    RPMIN3141 = 0x21,
    RPMIN2243 = 0x22,
    RPMIN1745 = 0x23,
    RPMIN1308 = 0x24,
    RPMIN0981 = 0x25,
    RPMIN0747 = 0x26,
    RPMIN0581 = 0x27,
    RPMIN0436 = 0x28,
    RPMIN0349 = 0x29,
    RPMIN0249 = 0x2a,
    RPMIN0193 = 0x2b,
    RPMIN0145 = 0x2c,
    RPMIN0109 = 0x2d,
    RPMIN0083 = 0x2e,
    RPMIN0064 = 0x2f,
    RPMIN0048 = 0x30,
    RPMIN0038 = 0x31,
    RPMIN0027 = 0x32,
    RPMIN0021 = 0x33,
    RPMIN0016 = 0x34,
    RPMIN0012 = 0x35,
    RPMIN0009 = 0x36,
    RPMIN0007 = 0x37,
    RPMIN0005 = 0x38,
    RPMIN0004 = 0x39,
    RPMIN0003 = 0x3a,
    RPMIN0002 = 0x3b,
    RPMIN0001p7 = 0x3c,
    RPMIN0001p3 = 0x3d,
    RPMIN0001p0 = 0x3e,
    RPMIN0000 = 0x3f
} LDC1000_rp_min_t;

// LDC_CONFIG
typedef enum {
    AMP_1V = 0x00,
    AMP_2V = (0x01<<3),
    AMP_4V = (0x02<<3)
} LDC1000_amplitude_t;

typedef enum {
    RESP_TIME_0192 = 0x02,
    RESP_TIME_0384 = 0x03,
    RESP_TIME_0768 = 0x04,
    RESP_TIME_1536 = 0x05,
    RESP_TIME_3072 = 0x06,
    RESP_TIME_6144 = 0x07
} LDC1000_resp_time_t;

// CLK_CONF
typedef enum {
    XIN = 0x00,
    XIN_XOUT = 0x02,
} LDC1000_clksel_t;

typedef enum {
    CLK_ON = 0x00,
    CLK_OFF = 0x01,
} LDC1000_clkpd_t;

// INTB_CONF
typedef enum {
    INTB_DIS = 0x00,
    WAKEUP_EN = 0x01,
    COMP_OUT = 0x02,
    DRDY_EN = 0x04
} LDC1000_intb_conf_t;

// POW_CONF
typedef enum {
    ACTIVE_MODE = 0x01,
    STBY_MODE = 0x00
} LDC1000_pow_conf_t;

// LDC_STATUS
typedef enum {
    OSC_STATUS_MASK = 0x80,
    DRDYB_MASK = 0x40,
    WAKEUP_DIS_MASK = 0x20,
    COMPB_MASK = 0x10
} LDC1000_status_t;

uint8_t LDC1000_DevId(SPI_Handle spi);
void LDC1000_Write_Rp_Max(SPI_Handle spi, uint8_t Val);
void LDC1000_Write_Rp_Min(SPI_Handle spi, uint8_t Val);
void LDC1000_Write_Min_Freq(SPI_Handle spi, uint8_t Val);
void LDC1000_Write_Conf(SPI_Handle spi, uint8_t Val);
void LDC1000_Write_Intb_Conf(SPI_Handle spi, uint8_t Val);
void LDC1000_Write_Clk_Conf(SPI_Handle spi, uint8_t Val);
void LDC1000_Write_Pow_Conf(SPI_Handle spi, uint8_t Val);
uint8_t LDC1000_Read_Status(SPI_Handle spi);
uint16_t LDC1000_Read_Proximity(SPI_Handle spi);
void LDC1000_Get_Proximity_Frame(SPI_Handle spi, uint16_t Samples, int32_t *DataSensor);
uint32_t LDC1000_Read_Fcount(SPI_Handle spi);
float RpCalc(uint32_t Proximity, float RpMax, float RpMin);

#endif /* LDC1000_H_ */
