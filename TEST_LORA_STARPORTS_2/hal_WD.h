/*
 * hal_WD.h
 *
 *  Created on: 24 jun. 2019
 *      Author: airizar
 */

#ifndef HAL_WD_H_
#define HAL_WD_H_

#include <ti/drivers/Watchdog.h>
#include <stddef.h>
#include <stdint.h>

Watchdog_Handle Startup_Watchdog(uint_least8_t index, uint32_t timeout);

#endif /* HAL_WD_H_ */
