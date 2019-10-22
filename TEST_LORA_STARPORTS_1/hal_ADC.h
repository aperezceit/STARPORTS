/*
 * hal_ADC.h
 *
 *  Created on: 15 abr. 2019
 *      Author: airizar
 */

#ifndef HAL_ADC_H_
#define HAL_ADC_H_

#include <ti/drivers/ADC.h>
#include <stdint.h>

ADC_Handle Startup_ADC(uint8_t index);

#endif /* HAL_ADC_H_ */
