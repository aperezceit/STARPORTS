/*
 * hal_Timer.h
 *
 *  Created on: 29 abr. 2019
 *      Author: airizar
 */

#include <ti/drivers/Timer.h>
#include <stdint.h>

void timerCallback(Timer_Handle myHandle);

Timer_Handle Startup_Timer(uint8_t index, uint32_t period) {

    Timer_Handle timer;
    Timer_Params params;

    /* Timer */
    Timer_init();
    Timer_Params_init(&params);
    params.period = period;
    params.periodUnits = Timer_PERIOD_US;
    params.timerMode = Timer_CONTINUOUS_CALLBACK;
    params.timerCallback = timerCallback;

    timer = Timer_open(index, &params);

    return timer;

}


void timerCallback(Timer_Handle myHandle) {

}
