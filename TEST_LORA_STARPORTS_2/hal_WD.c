/*
 * hal_WD.c
 *
 *  Created on: 24 jun. 2019
 *      Author: airizar
 */


#include <ti/drivers/I2C.h>
#include <ti/drivers/Watchdog.h>
#include <string.h>
#include "Board.h"
#include "hal_GPIO.h"

extern I2C_Handle i2c;

void watchdogCallback(uintptr_t watchdogHandle) {
    /*
     * If the Watchdog Non-Maskable Interrupt (NMI) is called,
     * set to '0' any output signal and disable the Node
     */

    I2C_close(i2c);
    I2C_As_GPIO_Low();  // Puts SCL and SDA signals low to save power
    Node_Disable();     // Auto Shutdown

}

Watchdog_Handle Startup_Watchdog(uint_least8_t index, uint32_t timeout) {

    uint32_t reloadValue;
    Watchdog_Params wdParams;
    Watchdog_Handle wd;

    Watchdog_init();
    Watchdog_Params_init(&wdParams);
    wdParams.callbackFxn = (Watchdog_Callback) watchdogCallback;
    wdParams.debugStallMode = Watchdog_DEBUG_STALL_ON;
    wdParams.resetMode = Watchdog_RESET_OFF;

    wd = Watchdog_open(index, &wdParams);
    if (wd == NULL) {
        /* Error opening Watchdog */
        while (1) {}
    }

    reloadValue = Watchdog_convertMsToTicks(wd, timeout);

    if (reloadValue != 0) {
        Watchdog_setReload(wd, reloadValue);
    }

    return wd;
}


