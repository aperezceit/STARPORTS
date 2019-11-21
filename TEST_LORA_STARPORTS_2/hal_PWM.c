/*
 * hal_PWM.c
 *
 *  Created on: 12 abr. 2019
 *      Author: airizar
 */
//dsfgtsa
#include <ti/drivers/PWM.h>
#include <stddef.h>

PWM_Handle Config_PWM(uint_least8_t index) {

    PWM_Handle pwm;
    PWM_Params params;

    /* Call driver init functions. */
    PWM_init();

    PWM_Params_init(&params);
    params.dutyUnits = PWM_DUTY_FRACTION;
    params.dutyValue = (PWM_DUTY_FRACTION_MAX>>1);
    params.periodUnits = PWM_PERIOD_HZ;
    params.periodValue = 8000000;
    pwm = PWM_open(index, &params);

    return pwm;

}
