/*
 * hal_GPIO.h
 *
 *  Created on: 25 may. 2019
 *      Author: airizar
 */

#ifndef HAL_GPIO_H_
#define HAL_GPIO_H_

uint8_t GPIO_Config(void);
uint8_t I2C_As_GPIO_Low(void);
uint8_t SPI_As_GPIO_Low(void);
void RN2483_Clear(void);
void RN2483_Set(void);
void ADXL355_Enable(void);
void ADXL355_Disable(void);
void BME280_Enable(void);
void BME280_Disable(void);
void LDC1000_Enable(void);
void LDC1000_Disable(void);
void Node_Enable(void);
void Node_Disable(void);
void ADXL355_SPI_Enable(void);
void ADXL355_SPI_Disable(void);
void BME280_SPI_Enable(void);
void BME280_SPI_Disable(void);
void LDC1000_SPI_Enable(void);
void LDC1000_SPI_Disable(void);
void SPI_CS_Disable(void);


#endif /* HAL_GPIO_H_ */
