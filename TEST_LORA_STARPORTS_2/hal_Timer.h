/*
 * hal_Timer.h
 *
 *  Created on: 29 abr. 2019
 *      Author: airizar
 */

#ifndef HAL_TIMER_H_
#define HAL_TIMER_H_

#include <ti/drivers/Timer.h>
#include <stdint.h>

Timer_Handle Startup_Continuous_Timer(uint8_t index, uint32_t period);

Timer_Handle Startup_Oneshot_Timer(uint8_t index, uint32_t interval);

void timer0Callback(Timer_Handle myHandle);

void timer1Callback(Timer_Handle myHandle);


#endif /* HAL_TIMER_H_ */
