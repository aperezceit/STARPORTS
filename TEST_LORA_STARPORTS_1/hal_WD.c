/*
 * hal_WD.c
 *
 *  Created on: 24 jun. 2019
 *      Author: airizar
 */


#include <ti/drivers/Watchdog.h>
#include <string.h>
#include "Board.h"


Watchdog_Handle Startup_Watchdog(uint_least8_t index, uint32_t timeout) {

    uint32_t reloadValue;
    Watchdog_Params wdParams;
    Watchdog_Handle wd;

    Watchdog_init();
    Watchdog_Params_init(&wdParams);
    wdParams.callbackFxn = NULL; // (Watchdog_Callback) watchdogCallback;
    wdParams.debugStallMode = Watchdog_DEBUG_STALL_ON;
    wdParams.resetMode = Watchdog_RESET_ON;

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

