/*
 * Copyright (c) 2015-2017, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ======== uartecho.c ========
 */
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <unistd.h>

/* Driver Header files */
#include <ti/drivers/UART.h>
#include <ti/drivers/I2C.h>
#include <ti/drivers/SPI.h>
#include <ti/drivers/Timer.h>
#include <ti/drivers/Power.h>
#include <ti/drivers/GPIO.h>
#include <ti/drivers/Watchdog.h>
#include <ti/drivers/net/wifi/wlan.h>
#include <ti/drivers/net/wifi/simplelink.h>

#include "platform.h"

#include <ti/devices/cc32xx/inc/hw_types.h>
#include <ti/devices/cc32xx/driverlib/prcm.h>

#include "hal_UART.h"
#include "hal_Timer.h"
#include "hal_GPIO.h"
#include "hal_WD.h"
#include "hal_I2C.h"
#include "hal_SPI.h"
#include "wifi.h"
#include "file_system.h"

#include "DS1374.h"

#include "Sensors.h"
#include "hal_LORA.h"

/* Example/Board Header files */
#include "Board.h"

#include "STARPORTS_App.h"

#define TIMEOUT_MS 10000 // 10 seconds Timeout

uint8_t Timer0Event = 0;
uint8_t Timer1Event = 0;
uint8_t LPDSOut = 0;

struct Node MyNode = {NODEID,1200,MODE_NORMAL_LORA,16,"PORTSN"};
struct TMP006_Data MyTMP006 = {TMP006_ID};
struct ADXL355_Data MyADXL = {ADXL355_ID,500,256};
struct BME280_Data MyBME = {BME280_ID};
struct LDC1000_Data MyLDC = {LDC1000_ID,16};
struct Vbat_Data MyVbat = {VBAT_ID,16};
/*
 *  ======== mainThread ========
 */

void WakeupFromLPDS(uint_least8_t);

UART_Handle uart0;  // uart0 is now global to access the handle in UART_PRINT kind debug messages
                    // Can be removed in final deployment
I2C_Handle i2c;

SPI_Handle spi;

Watchdog_Handle wd; // Watchdog Handle

void *mainThread(void *arg0)
{
    /* UART Handles */
    UART_Handle uart1;

    /* RTC */

    unsigned char Mess[32];

    uint8_t DataPacket[256];
    uint8_t DataPacketLen;

    struct LoraNode MyLoraNode;

    unsigned char buf[256];

    uint8_t ret;
    uint8_t sz;
    uint8_t NTxFail;
    uint8_t NextStep = CONTINUE;

    /* wifi var */
    int16_t         sockId;
    int16_t         status = -1;


    // Quicky enable de node (the jumper an be removed)
    GPIO_setConfig(Board_EN_NODE, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_HIGH);
    Node_Enable();

    // Configure the rest of GPIO pins
    ret = GPIO_Config();

    /* Open a Watchdog driver instance */
    // wd = Startup_Watchdog(Board_WATCHDOG0, TIMEOUT_MS);

    uart0 = Startup_UART(Board_UART0, 115200); // arrancado antes de leer para debug

    // st_listFiles(0);

    /************* Begin Configure Peripherals ***********************/
    // Reset the RN2483
    RN2483_Clear();
    uart1 = Startup_UART(Board_UART1, 57600);
    /************* End Configure Peripherals ***********************/

    UART_write(uart0, "\r\nInitiating Test of STARPORTS...\r\n", 35);

    /************** Begin Configuration and Setup Wireless Connectivity ***************/

    RN2483_Set();                   // Set /MCLR Pin to 1 releases RN2483
    sz = GetLine_UART(uart1, buf);  // Recoge texto de inicialización del RN2483
    if (sz==0) {
        // Buffer size is too large
    }

    strcpy(MyLoraNode.DevEui,"0004A30B00EAEF51");
    strcpy(MyLoraNode.AppEui,"70B3D57ED0024061");
    strcpy(MyLoraNode.AppKey,"9FD3209434E591823337247216DAF46A");

    Setup_Otaa_Lora(uart1,&MyLoraNode);

    Join_Otaa_Lora(uart1);


}

void WakeupFromLPDS(uint_least8_t var) {
    LPDSOut = 1;
}


