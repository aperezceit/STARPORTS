/*
 * hal_GPIO.c
 *
 *  Created on: 25 may. 2019
 *      Author: airizar
 */

#include <ti/drivers/GPIO.h>
#include <unistd.h>
#include "Board.h"


uint8_t GPIO_Config(void) {


    // /MCLR Pin Configured, and set at Low
    GPIO_setConfig(Board_RN2483_MCLR,    GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);
    // Selects the SPI device to interface (Active Low)
    GPIO_setConfig(Board_ADXL355_CS,     GPIO_CFG_OUT_STD | GPIO_CFG_OUT_HIGH); // Provisionally LOW
    GPIO_setConfig(Board_BME280_CS,      GPIO_CFG_OUT_STD | GPIO_CFG_OUT_HIGH);
    GPIO_setConfig(Board_LDC1000_CS,     GPIO_CFG_OUT_STD | GPIO_CFG_OUT_HIGH);

    // Enable the individual LDO for sensor (active high)
    GPIO_setConfig(Board_EN_ADXL355,     GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);
    GPIO_setConfig(Board_EN_BME280,      GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);
    GPIO_setConfig(Board_EN_LDC1000,     GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);
    GPIO_setConfig(Board_EN_NODE,        GPIO_CFG_OUT_STD | GPIO_CFG_OUT_HIGH);
    // INTB Inputs from DS1374
    GPIO_setConfig(Board_DS1374_INTB,    GPIO_CFG_IN_NOPULL);

    return 0;

}

uint8_t I2C_As_GPIO_Low(void) {

    GPIO_setConfig(Board_SCL_ASGPIO,    GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);
    usleep(1000);   // Importante para evitar una condición de START en el RTC (ver datasheet DS1374)
    GPIO_setConfig(Board_SDA_ASGPIO,    GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);

    return 0;

}

uint8_t SPI_As_GPIO_Low(void) {

    GPIO_setConfig(Board_SCLK_ASGPIO,    GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);
    GPIO_setConfig(Board_MOSI_ASGPIO,    GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);
    GPIO_setConfig(Board_CS_ASGPIO,      GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);

    return 0;

}


void RN2483_Clear(void) {

    GPIO_write(Board_RN2483_MCLR,0);

}


void RN2483_Set(void) {

    GPIO_write(Board_RN2483_MCLR,1);

}

void ADXL355_Enable(void) {

    GPIO_write(Board_EN_ADXL355,1);

}


void ADXL355_Disable(void) {

    GPIO_write(Board_EN_ADXL355,0);

}

void BME280_Enable(void) {

    GPIO_write(Board_EN_BME280,1);

}


void BME280_Disable(void) {

    GPIO_write(Board_EN_BME280,0);

}

void LDC1000_Enable(void) {

    GPIO_write(Board_EN_LDC1000,1);

}


void LDC1000_Disable(void) {

    GPIO_write(Board_EN_LDC1000,0);

}

void Node_Enable(void) {

    GPIO_write(Board_EN_NODE,1);

}


void Node_Disable(void) {

    GPIO_write(Board_EN_NODE,0);

}

void ADXL355_SPI_Enable(void) {

    GPIO_write(Board_ADXL355_CS,0);

}


void ADXL355_SPI_Disable(void) {

    GPIO_write(Board_ADXL355_CS,1);

}

void BME280_SPI_Enable(void) {

    GPIO_write(Board_BME280_CS,0);

}


void BME280_SPI_Disable(void) {

    GPIO_write(Board_BME280_CS,1);

}

void LDC1000_SPI_Enable(void) {

    GPIO_write(Board_LDC1000_CS,0);

}


void LDC1000_SPI_Disable(void) {

    GPIO_write(Board_LDC1000_CS,1);

}

void SPI_CS_Disable(void) {

    GPIO_write(Board_LDC1000_CS,1);
    GPIO_write(Board_BME280_CS,1);
    GPIO_write(Board_ADXL355_CS,1);

}
