/*
 * hal_ADC.c
 *
 *  Created on: 15 abr. 2019
 *      Author: airizar
 */

#include <ti/drivers/ADC.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>


ADC_Handle Startup_ADC(uint8_t index) {

    ADC_Handle adc;
    ADC_Params params;

    ADC_Params_init(&params);
    ADC_init();
    adc = ADC_open(index, &params);

    return adc;

}
