/*
 * hal_Timer.h
 *
 *  Created on: 29 abr. 2019
 *      Author: airizar
 */

#include <ti/drivers/Timer.h>
#include <ti/drivers/Power.h>
#include <stdint.h>
#include "STARPORTS_App.h"

extern uint8_t Timer0Event;
void timer0Callback(Timer_Handle myHandle) {
    Timer0Event = 1;
}

extern uint8_t Timer1Event;
void timer1Callback(Timer_Handle myHandle) {
    Timer1Event = 1;
}

Timer_Handle Startup_Continuous_Timer(uint8_t index, uint32_t period) {

    Timer_Handle timer;
    Timer_Params params;

    /* Timer */
    Timer_Params_init(&params);
    params.period = period;
    params.periodUnits = Timer_PERIOD_US;
    params.timerMode = Timer_CONTINUOUS_CALLBACK;
    params.timerCallback = timer0Callback;

    timer = Timer_open(index, &params);

    return timer;

}


Timer_Handle Startup_Oneshot_Timer(uint8_t index, uint32_t interval) {

    Timer_Handle timer;
    Timer_Params params;

    /* Timer */
    Timer_Params_init(&params);
    params.period = interval;
    params.periodUnits = Timer_PERIOD_US;
    params.timerMode = Timer_ONESHOT_CALLBACK;
    params.timerCallback = timer1Callback;

    timer = Timer_open(index, &params);

    return timer;

}


