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
#include <STARPORTS_GetLoraId.h>
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


#define TIMEOUT_MS 10000 // 10 seconds Timeout

#define MYRTC 1

uint8_t Timer0Event = 0;
uint8_t Timer1Event = 0;
uint8_t LPDSOut = 0;

struct Node MyNode = {NODEID,1200,MODE_NORMAL_LORA,16,"PORTS0"};
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

    uint8_t FirstOTAA=1;
    int upctr;

    /* Timer Handle */
    Timer_Handle timer0;

    /* UART Handles */
    UART_Handle uart1;

    /* RTC */

    unsigned char Mess[32];

    struct LoraNode MyLoraNode;

    unsigned char Command[128];
    unsigned char buf[64];
    uint8_t ret;
    uint8_t sz;
    uint8_t NTxFail;

    /* wifi var */
    int16_t         sockId;
    int16_t         status = -1;

    sl_Stop(0);

    /* Open a Watchdog driver instance */
    // wd = Startup_Watchdog(Board_WATCHDOG0, TIMEOUT_MS);

    // Power_enablePolicy();

    /* Get Param Values from internal filesystem */
    // Get MyNode.WakeUpInterval --> DS1374_Write_WdAlmb(i2c, MyNode.WakeUpInterval);
    // Get MyNode.Mode --> Determine Wireless Interface
    // Get MyNode.NCycles --> Get Value only if Mode is Storage
    // Get MyNode.SSID[] --> Get Value only if Mode is WiFi


    /* GPIO */
    // GPIOs Configured and set as outputs in Low state
    // GPIO_setConfig(Board_TIME_MEAS, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);
    // GPIO_write(Board_TIME_MEAS,1);
    ret = GPIO_Config();

    Node_Enable();

    RN2483_Clear();
    usleep(10000);

    /* UART0 is for sending debug information */
    uart0 = Startup_UART(Board_UART0, 115200);
    /* UART1 connects to RN2483 */
    uart1 = Startup_UART(Board_UART1, 57600);

    /* FYLE SYSTEM*/
    //Muestra estado memoria y lista de archivos
    sl_Start(0, 0, 0); // necesario arrancar device antes de trabajar con file system
    // st_showStorageInfo(); //no es necesario ver la info, se puede omitir
    // st_listFiles(1);
    // newFile();

    UART_write(uart0, "\r\nInitiating STARPORTS...\r\n", 27);

    RN2483_Set();
    // Recoge texto de inicialización del RN2483
    sz = GetLine_UART(uart1, buf);
    if (sz==0) {
        // Buffer size is too large
    }

    // MyLoraNode.PortNoTx = 1;
    // Keys for Device1 (TTN)
    // strcpy(MyLoraNode.AppEui, "70B3D57ED0019CD5");
    // strcpy(MyLoraNode.AppKey, "C4C5FAAD6EAFFD04936B24E6B3E9B794");
    // Keys for Device2 (TTN)
    // mac set appeui 70B3D57ED0019CD5
    // mac set devaddr 26011B6E
    // mac set appkey C4C5FAAD6EAFFD04936B24E6B3E9B794
    // mac set appskey B61E3BB55A1F2E0DD5C33E72261D69D6
    // mac set nwkskey 68CFC0712E1E33AC69FB712EF556851A


    // strcpy(MyLoraNode.AppEui, "70B3D57ED0019CD5");
    // strcpy(MyLoraNode.AppKey, "358D1977FCA3717CDE76377508943AA6");

    // strcpy(MyLoraNode.AppsKey, "D4B977D54A7ECC562CD18031178D1D43");
    // strcpy(MyLoraNode.NwksKey, "D7AB91C4AE6F5A09C17540145556AD74");

    strcpy(Command,"sys get hweui\r\n");
    UART_write(uart1, (const char *)(Command), 15);
    sz = GetLine_UART(uart1, buf);
    strncpy(&MyLoraNode.DevEui,buf,16);

    UART_write(uart0,"DevId: ",7);
    UART_write(uart0,&MyLoraNode.DevEui,16);
    UART_write(uart0,"\r\n",2);


    strcpy(Command,"mac set deveui ");
    strncat(Command, &MyLoraNode.DevEui,16);
    strcat(Command, "\r\n");
    UART_write(uart1, (const char *)Command, 33);
    sz = GetLine_UART(uart1, buf);
    if (strncmp(buf,"ok",2)==0) {
        UART_write(uart0,"mac set deveui successful\r\n",27);
    }


    /* Enter AppEui */
    UART_write(uart0,"Enter AppEui: ",14);
    sz = GetLine_UART(uart0, buf);
    strncpy(MyLoraNode.AppEui, buf,16);

    UART_write(uart0, "AppEui entered is ", 18);
    UART_write(uart0, &MyLoraNode.AppEui, 16);
    UART_write(uart0,"\r\n",2);

    strcpy(Command,"mac set appeui ");
    strncat(Command, &MyLoraNode.AppEui,16);
    strcat(Command, "\r\n");
    UART_write(uart1, (const char *)Command, 33);
    sz = GetLine_UART(uart1, buf);
    if (strncmp(buf,"ok",2)==0) {
        UART_write(uart0,"mac set appeui successful\r\n",27);
    }


    /* Enter AppKey */
    UART_write(uart0,"Enter AppKey: ",14);
    sz = GetLine_UART(uart0, buf);
    strncpy(MyLoraNode.AppKey, buf,32);

    UART_write(uart0, "AppKey entered is ", 18);
    UART_write(uart0, &MyLoraNode.AppKey, 32);
    UART_write(uart0,"\r\n",2);

    strcpy(Command,"mac set appkey ");
    strncat(Command, &MyLoraNode.AppKey,16);
    strcat(Command, "\r\n");
    UART_write(uart1, (const char *)Command, 33);
    sz = GetLine_UART(uart1, buf);
    if (strncmp(buf,"ok",2)==0) {
        UART_write(uart0,"mac set appkey successful\r\n",27);
    }

    /* Save the results to RN2384 EEPROM */
    ret = Mac_Save(uart1);
    if (ret==0) {
        UART_write(uart0,"mac save successful\r\n",21);
    }

}


void WakeupFromLPDS(uint_least8_t var) {
    LPDSOut = 1;
}


