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

#define TIMEOUT_MS 40000                                                /* 40 seconds Timeout */

uint8_t Timer0Event = 0;
uint8_t Timer1Event = 0;
uint8_t LPDSOut = 0;

struct Node MyNode = {NODEID,1200,MODE_NORMAL_LORA,16,"PORTS2"};
struct TMP006_Data MyTMP006 = {TMP006_ID};
struct ADXL355_Data MyADXL = {ADXL355_ID,500,256};
struct BME280_Data MyBME = {BME280_ID};
struct LDC1000_Data MyLDC = {LDC1000_ID,16};
struct Vbat_Data MyVbat = {VBAT_ID,16};
/*
 *  ======== mainThread ========
 */

void WakeupFromLPDS(uint_least8_t);

UART_Handle uart0;                                                              /* uart0 is now global to access the handle in UART_PRINT kind debug messages.Can be removed in final deployment */
I2C_Handle i2c;
SPI_Handle spi;
Watchdog_Handle wd;                                                             /* Watchdog Handle */

void *mainThread(void *arg0)
{
    /* UART Handles */
    UART_Handle uart1;

    /* RTC */
    unsigned char Mess[32];
    unsigned char Command[64];

    uint8_t DataPacket[256];
    uint8_t DataPacketLen;

    struct LoraNode MyLoraNode;

    unsigned char buf[256];
    uint8_t macAddress[SL_MAC_ADDR_LEN];
    uint8_t macAddressLen = SL_MAC_ADDR_LEN;

    uint8_t ret;
    uint8_t sz;
    uint8_t NTxFail;
    uint8_t NextStep = CONTINUE;
    uint8_t mask;
    uint16_t nodeId;

    /* wifi var */
    int16_t         sockId;
    int16_t         status = -1;

    GPIO_setConfig(Board_EN_NODE, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_HIGH);        /* Quicky enable de node (the jumper can be removed) */
    Node_Enable();

    Watchdog_init();

    ret = GPIO_Config();                                                        /* Configure the rest of GPIO pins */
    wd = Startup_Watchdog(Board_WATCHDOG0, TIMEOUT_MS);                         /* Open a Watchdog driver instance */

    /************ Begin Read Configuration Files **************************/
    SPI_init();                                                                 /* necesario inicializar SPI antes de sl_Start */
    sl_Start(0, 0, 0);                                                          /*necesario para poder trabajar con FS */

//    uart0 = Startup_UART(Board_UART0, 115200);                                  /* arrancado antes de leer para debug */
/*
#ifdef DEBUG
    UART_write(uart0, "\r\nInitiating Test of STARPORTS...\r\n", 35);
#endif
*/
    /* Get Param Values from internal filesystem */
    MyLoraNode.PortNoTx = 1;
    //MyNode.NodeId = st_readFileNodeId();                                      /*  Get MyNodeId */
    MyNode.WakeUpInterval = st_readFileWakeUp();                                /* Get MyNode.WakeUpInterval --> Read WakeUp_Time File */
    MyNode.Mode = st_readFileMode();                                            /* Get MyNode.Mode --> Read File Mode */
    MyNode.NCycles = st_readFileNCycles();                                      /* Get MyNode.NCycles --> Read File Ncycles */
//    st_readFileSSID(&(MyNode.SSID));       NOT NECCESARY FOR TEST             /* Get MyNode.SSID[] --> Read SSID File */
//    MyNode.NBoot = st_readFileNBoot();     NOT NECCESARY FOR TEST             /* Get MyNode.NBoot --> Read NBoot File: (1) or (0) */
    MyNode.NFails = 0   ;                                                       /* Get MyNode.NFails --> Number of failed attempts to Wireless Connection */
    MyNode.Payload = st_readFilePayload();                                      /* Get MyNode.Payloads --> selects the payload to send */
    MyNode.Cycles = st_readFileCycles();                                        /* Get MyNode.Cycles -- > number of times sending the same paylaod */
    MyNode.Iter = st_readFileIter();                                            /* Get MyNode.Iter -- > payload variable  */
    MyNode.Count = st_readFileCount();                                          /* Get MyNode.Count --> payload variable */
    /************ Ends Reading Configuration Files **************************/
/* NOT NECCESARY FOR TEST
    if (MyNode.NFails>=4) {

#ifdef DEBUG
        UART_PRINT("NFails > 4, SETTING NODE IN WIFI MODE\n\r");
#endif

        usleep(2000);
        wlanConf();                                                             /* Setup Node as WiFi and connect to Known Host */
/*        wlanConnectFromFile(MyNode.SSID);
        usleep(2000);

        MyNode.NBoot=0;
        writeNBoot(MyNode.NBoot);
        MyNode.NFails=0;
        writeNFails(MyNode.NFails);
    }
*/
    /************* Begin Configure Peripherals ***********************/
    RN2483_Clear();                                                             /* Reset the RN2483 */
    i2c = Startup_I2C(Board_I2C0);                                              /* I2C interface started */
    /************* End Configure Peripherals ***********************/

    /*************** Begin Setting Node Configuration Parameters */
    DS1374_Write_Ctrl(i2c);                                                     /* Configures the RTC */
    DS1374_Clear_AF(i2c);
    DS1374_Write_WdAlmb(i2c, MyNode.WakeUpInterval);                            /* Set the WakeUp_Time in RTC */

    /************** Begin Configuration and Setup Wireless Connectivity ***************/
    uart1 = Startup_UART(Board_UART1, 57600);                                   /* UART1 connects to RN2483 */

    if (MyNode.Mode==MODE_NORMAL_LORA) {
        RN2483_Set();                                                           /* Set /MCLR Pin to 1 releases RN2483 */
        sz = GetLine_UART(uart1, buf);                                          /* Recoge texto de inicialización del RN2483 */
        if (sz==0){
                                                                                /* Buffer size is too large */
        }
        MyLoraNode.Upctr = st_readFileUpCntr();                                 /* Read upctr from file */
        Mac_Set_Upctr(uart1,&MyLoraNode);
        MyLoraNode.Dnctr = st_readFileDnCntr();
        Mac_Set_Dnctr(uart1,&MyLoraNode);
        // Mac_Set_Pwridx(uart1, 1);                                            /* Set Tx Power (by default is 1, the maximum 14dBm) */
        // Mac_Adr_On(uart1);                                                   /* Set adaptive datarate ON (useful when using OTAA) */
        Mac_Ar_On(uart1);                                                       /* Set Automatic Retransmit ON */

        /********** Join ABP ***************/
        ret = Join_Abp_Lora(uart1);
        if (ret==SUCCESS_ABP_LORA) {
//            MyNode.NFails=0;
//            writeNFails(MyNode.NFails);
            // Continue reading sensors
        } else {
/*
#ifdef DEBUG
            UART_write(uart0,"Join_Abp_Lora() Failed",22);
            UART_write(uart0, "\r\n", 2);
#endif
*/
//            MyNode.NFails++;                                                    /*  Write NFails File */
//            writeNFails(MyNode.NFails);
            NextStep=SHUTDOWN;
        }

    } else if (MyNode.Mode==MODE_NORMAL_WIFI) {
            //st_readFileSSID(&(MyNode.SSID));     NOT NECCESARY FOR TEST       /*  Read SSID from file */
            // Read other params ...
            // usleep(2000);
            ret = wlanConf();                                                   /* Setup WiFi */
            ret = wlanConnectFromFile(MyNode.SSID);                             /* Connect WiFi */
            usleep(2000);

            if (ret==SUCCESS_CONNECT_WIFI) {
//                MyNode.NFails=0;
//                writeNFails(MyNode.NFails);
//                UART_PRINT("SUCCESS WIFI CONNECTION\n\r");
                // Continue reading sensors
            } else {
//                MyNode.NFails++;
//                writeNFails(MyNode.NFails);
//                UART_PRINT("ERROR CONNECTING WIFI\n\r");
                NextStep=SHUTDOWN;
            }

    }
    /************** End of Configuration and Setup Wireless Connectivity ***************/

    if (NextStep==SHUTDOWN) {
        I2C_close(i2c);
        I2C_As_GPIO_Low();                                                      /* Puts SCL and SDA signals low to save power*/
        Node_Disable();                                                         /*  Auto Shutdown */
    }

    /************** Begin Reading Data from Sensors ***********************************/
    SPI_CS_Disable();                                                           /* Put CS to Logic High */
    spi = Startup_SPI(Board_SPI_MASTER, 8, 5000000);                            /* Configure SPI Master at 5 Mbps, 8-bits, CPOL=0, PHA=0 */
    Timer_init();                                                               /* Start Timer for ADC Timestep */
//    DataPacketLen = GetSensorData(DataPacket);        NOT NECCESARY FOR TEST
    /************** End Reading Data from Sensors ***********************************/

    /* Close all sensor related peripherals */
    SPI_close(spi);
    // Not really enable the SPI but just puts the CS signal to '0'
    // to prevent PCB from not really shutting down (a high value in some SPI signals
    // keeps sensor chips active)
    LDC1000_SPI_Enable();
    ADXL355_SPI_Enable();
    BME280_SPI_Enable();
    SPI_As_GPIO_Low();                                                          /* Puts all rest of SPI signals to '0' to save power */
    I2C_close(i2c);
    I2C_As_GPIO_Low();                                                          /* Puts SCL and SDA signals low to save power */

//    MyLoraNode.DataLenTx = Uint8Array2Char(DataPacket, DataPacketLen, &(MyLoraNode.DataTx));  NOT NECCESARY FOR TEST

    /*
     * test paper
     */

    memset(MyLoraNode.DataTx,'/0', strlen(MyLoraNode.DataTx));
//    UART_write(uart0, &(MyLoraNode.DataTx),MyLoraNode.DataLenTx);
//    UART_write(uart0, "\r\n", 2);

    if(MyNode.Payload==0){
//        UART_PRINT("PAY 0\r\n");
        strcpy(MyLoraNode.DataTx,PAYLOAD_0);
        MyLoraNode.DataLenTx = strlen(MyLoraNode.DataTx);
//        UART_write(uart0, &(MyLoraNode.DataTx),MyLoraNode.DataLenTx);
//        UART_write(uart0, "\r\n", 2);

    }else if(MyNode.Payload==1){
//        UART_PRINT("PAY 1\r\n");
        strcpy(MyLoraNode.DataTx,PAYLOAD_1);
        MyLoraNode.DataLenTx = strlen(MyLoraNode.DataTx);
//        UART_write(uart0, &(MyLoraNode.DataTx),MyLoraNode.DataLenTx);
//        UART_write(uart0, "\r\n", 2);

    }else if(MyNode.Payload==2){
//        UART_PRINT("PAY 2\r\n");
        strcpy(MyLoraNode.DataTx,PAYLOAD_2);
        MyLoraNode.DataLenTx = strlen(MyLoraNode.DataTx);
//        UART_write(uart0, &(MyLoraNode.DataTx),MyLoraNode.DataLenTx);
//        UART_write(uart0, "\r\n", 2);

    }else if(MyNode.Payload==3){
//        UART_PRINT("PAY 3\r\n");
        strcpy(MyLoraNode.DataTx,PAYLOAD_3);
        MyLoraNode.DataLenTx = strlen(MyLoraNode.DataTx);
//        UART_write(uart0, &(MyLoraNode.DataTx),MyLoraNode.DataLenTx);
//        UART_write(uart0, "\r\n", 2);

    }else if(MyNode.Payload==4){
//        UART_PRINT("PAY 4\r\n");
        strcpy(MyLoraNode.DataTx,PAYLOAD_4);
        MyLoraNode.DataLenTx = strlen(MyLoraNode.DataTx);
//        UART_write(uart0, &(MyLoraNode.DataTx),MyLoraNode.DataLenTx);
//        UART_write(uart0, "\r\n", 2);

    }else if(MyNode.Payload==5){
//        UART_PRINT("PAY 5\r\n");
        strcpy(MyLoraNode.DataTx,PAYLOAD_5);
        MyLoraNode.DataLenTx = strlen(MyLoraNode.DataTx);
//        UART_write(uart0, &(MyLoraNode.DataTx),MyLoraNode.DataLenTx);
//        UART_write(uart0, "\r\n", 2);

    }else if(MyNode.Payload==6){
//        UART_PRINT("PAY 6\r\n");
        strcpy(MyLoraNode.DataTx,PAYLOAD_6);
        MyLoraNode.DataLenTx = strlen(MyLoraNode.DataTx);
//        UART_write(uart0, &(MyLoraNode.DataTx),MyLoraNode.DataLenTx);
//        UART_write(uart0, "\r\n", 2);

    }else if(MyNode.Payload==7){
//        UART_PRINT("PAY 7\r\n");
        strcpy(MyLoraNode.DataTx,PAYLOAD_7);
        MyLoraNode.DataLenTx = strlen(MyLoraNode.DataTx);
//        UART_write(uart0, &(MyLoraNode.DataTx),MyLoraNode.DataLenTx);
//        UART_write(uart0, "\r\n", 2);

    }else if(MyNode.Payload==8){
//        UART_PRINT("PAY 8\r\n");
        strcpy(MyLoraNode.DataTx,PAYLOAD_8);
        MyLoraNode.DataLenTx = strlen(MyLoraNode.DataTx);
//        UART_write(uart0, &(MyLoraNode.DataTx),MyLoraNode.DataLenTx);
//        UART_write(uart0, "\r\n", 2);

    }else if(MyNode.Payload==9){
//        UART_PRINT("PAY 9\r\n");
        strcpy(MyLoraNode.DataTx,PAYLOAD_9);
        MyLoraNode.DataLenTx = strlen(MyLoraNode.DataTx);
//        UART_write(uart0, &(MyLoraNode.DataTx),MyLoraNode.DataLenTx);
//        UART_write(uart0, "\r\n", 2);

    }else if(MyNode.Payload==10 && MyNode.Iter==0){
//        UART_PRINT("PAY 10\r\n");
        if(MyNode.Count < MyNode.Cycles){
            strcpy(MyLoraNode.DataTx,PAYLOAD_0);
            MyLoraNode.DataLenTx = strlen(MyLoraNode.DataTx);
//            UART_write(uart0, &(MyLoraNode.DataTx),MyLoraNode.DataLenTx);
//            UART_write(uart0, "\r\n", 2);
            MyNode.Count ++;
            writeCount(MyNode.Count);
        }else{
            strcpy(MyLoraNode.DataTx,PAYLOAD_0);
            MyLoraNode.DataLenTx = strlen(MyLoraNode.DataTx);
//            UART_write(uart0, &(MyLoraNode.DataTx),MyLoraNode.DataLenTx);
//            UART_write(uart0, "\r\n", 2);
            MyNode.Count = 0;
            writeCount(MyNode.Count);
            MyNode.Iter ++;
            writeIter(MyNode.Iter);
        }
    }else if(MyNode.Payload==10 && MyNode.Iter==1){
        if(MyNode.Count < MyNode.Cycles){
            strcpy(MyLoraNode.DataTx,PAYLOAD_1);
            MyLoraNode.DataLenTx = strlen(MyLoraNode.DataTx);
//            UART_write(uart0, &(MyLoraNode.DataTx),MyLoraNode.DataLenTx);
//            UART_write(uart0, "\r\n", 2);
            MyNode.Count ++;
            writeCount(MyNode.Count);
        }else{
            strcpy(MyLoraNode.DataTx,PAYLOAD_1);
            MyLoraNode.DataLenTx = strlen(MyLoraNode.DataTx);
//            UART_write(uart0, &(MyLoraNode.DataTx),MyLoraNode.DataLenTx);
//            UART_write(uart0, "\r\n", 2);
            MyNode.Count = 0;
            writeCount(MyNode.Count);
            MyNode.Iter ++;
            writeIter(MyNode.Iter);
        }
    }else if(MyNode.Payload==10 && MyNode.Iter==2){
        if(MyNode.Count < MyNode.Cycles){
            strcpy(MyLoraNode.DataTx,PAYLOAD_2);
            MyLoraNode.DataLenTx = strlen(MyLoraNode.DataTx);
//            UART_write(uart0, &(MyLoraNode.DataTx),MyLoraNode.DataLenTx);
//            UART_write(uart0, "\r\n", 2);
            MyNode.Count ++;
            writeCount(MyNode.Count);
        }else{
            strcpy(MyLoraNode.DataTx,PAYLOAD_2);
            MyLoraNode.DataLenTx = strlen(MyLoraNode.DataTx);
//            UART_write(uart0, &(MyLoraNode.DataTx),MyLoraNode.DataLenTx);
//            UART_write(uart0, "\r\n", 2);
            MyNode.Count = 0;
            writeCount(MyNode.Count);
            MyNode.Iter ++;
            writeIter(MyNode.Iter);
        }
    }else if(MyNode.Payload==10 && MyNode.Iter==3){
        if(MyNode.Count < MyNode.Cycles){
            strcpy(MyLoraNode.DataTx,PAYLOAD_3);
            MyLoraNode.DataLenTx = strlen(MyLoraNode.DataTx);
//            UART_write(uart0, &(MyLoraNode.DataTx),MyLoraNode.DataLenTx);
//            UART_write(uart0, "\r\n", 2);
            MyNode.Count ++;
            writeCount(MyNode.Count);
        }else{
            strcpy(MyLoraNode.DataTx,PAYLOAD_3);
            MyLoraNode.DataLenTx = strlen(MyLoraNode.DataTx);
//            UART_write(uart0, &(MyLoraNode.DataTx),MyLoraNode.DataLenTx);
//            UART_write(uart0, "\r\n", 2);
            MyNode.Count = 0;
            writeCount(MyNode.Count);
            MyNode.Iter ++;
            writeIter(MyNode.Iter);
        }
    }else if(MyNode.Payload==10 && MyNode.Iter==4){
        if(MyNode.Count < MyNode.Cycles){
            strcpy(MyLoraNode.DataTx,PAYLOAD_4);
            MyLoraNode.DataLenTx = strlen(MyLoraNode.DataTx);
//            UART_write(uart0, &(MyLoraNode.DataTx),MyLoraNode.DataLenTx);
//            UART_write(uart0, "\r\n", 2);
            MyNode.Count ++;
            writeCount(MyNode.Count);
        }else{
            strcpy(MyLoraNode.DataTx,PAYLOAD_4);
            MyLoraNode.DataLenTx = strlen(MyLoraNode.DataTx);
//            UART_write(uart0, &(MyLoraNode.DataTx),MyLoraNode.DataLenTx);
//            UART_write(uart0, "\r\n", 2);
            MyNode.Count = 0;
            writeCount(MyNode.Count);
            MyNode.Iter ++;
            writeIter(MyNode.Iter);
        }
    }else if(MyNode.Payload==10 && MyNode.Iter==5){
        if(MyNode.Count < MyNode.Cycles){
            strcpy(MyLoraNode.DataTx,PAYLOAD_5);
            MyLoraNode.DataLenTx = strlen(MyLoraNode.DataTx);
//            UART_write(uart0, &(MyLoraNode.DataTx),MyLoraNode.DataLenTx);
//            UART_write(uart0, "\r\n", 2);
            MyNode.Count ++;
            writeCount(MyNode.Count);
        }else{
            strcpy(MyLoraNode.DataTx,PAYLOAD_5);
            MyLoraNode.DataLenTx = strlen(MyLoraNode.DataTx);
//            UART_write(uart0, &(MyLoraNode.DataTx),MyLoraNode.DataLenTx);
//            UART_write(uart0, "\r\n", 2);
            MyNode.Count = 0;
            writeCount(MyNode.Count);
            MyNode.Iter ++;
            writeIter(MyNode.Iter);
        }
    }else if(MyNode.Payload==10 && MyNode.Iter==6){
        if(MyNode.Count < MyNode.Cycles){
            strcpy(MyLoraNode.DataTx,PAYLOAD_6);
            MyLoraNode.DataLenTx = strlen(MyLoraNode.DataTx);
//            UART_write(uart0, &(MyLoraNode.DataTx),MyLoraNode.DataLenTx);
//            UART_write(uart0, "\r\n", 2);
            MyNode.Count ++;
            writeCount(MyNode.Count);
        }else{
            strcpy(MyLoraNode.DataTx,PAYLOAD_6);
            MyLoraNode.DataLenTx = strlen(MyLoraNode.DataTx);
//            UART_write(uart0, &(MyLoraNode.DataTx),MyLoraNode.DataLenTx);
//            UART_write(uart0, "\r\n", 2);
            MyNode.Count = 0;
            writeCount(MyNode.Count);
            MyNode.Iter ++;
            writeIter(MyNode.Iter);
        }
    }else if(MyNode.Payload==10 && MyNode.Iter==7){
        if(MyNode.Count < MyNode.Cycles){
            strcpy(MyLoraNode.DataTx,PAYLOAD_7);
            MyLoraNode.DataLenTx = strlen(MyLoraNode.DataTx);
//            UART_write(uart0, &(MyLoraNode.DataTx),MyLoraNode.DataLenTx);
//            UART_write(uart0, "\r\n", 2);
            MyNode.Count ++;
            writeCount(MyNode.Count);
        }else{
            strcpy(MyLoraNode.DataTx,PAYLOAD_7);
            MyLoraNode.DataLenTx = strlen(MyLoraNode.DataTx);
//            UART_write(uart0, &(MyLoraNode.DataTx),MyLoraNode.DataLenTx);
//            UART_write(uart0, "\r\n", 2);
            MyNode.Count = 0;
            writeCount(MyNode.Count);
            MyNode.Iter ++;
            writeIter(MyNode.Iter);
        }
    }else if(MyNode.Payload==10 && MyNode.Iter==8){
        if(MyNode.Count < MyNode.Cycles){
            strcpy(MyLoraNode.DataTx,PAYLOAD_8);
            MyLoraNode.DataLenTx = strlen(MyLoraNode.DataTx);
//            UART_write(uart0, &(MyLoraNode.DataTx),MyLoraNode.DataLenTx);
//            UART_write(uart0, "\r\n", 2);
            MyNode.Count ++;
            writeCount(MyNode.Count);
        }else{
            strcpy(MyLoraNode.DataTx,PAYLOAD_8);
            MyLoraNode.DataLenTx = strlen(MyLoraNode.DataTx);
//            UART_write(uart0, &(MyLoraNode.DataTx),MyLoraNode.DataLenTx);
//            UART_write(uart0, "\r\n", 2);
            MyNode.Count = 0;
            writeCount(MyNode.Count);
            MyNode.Iter ++;
            writeIter(MyNode.Iter);
        }
    }else if(MyNode.Payload==10 && MyNode.Iter==9){
        if(MyNode.Count < MyNode.Cycles){
            strcpy(MyLoraNode.DataTx,PAYLOAD_9);
            MyLoraNode.DataLenTx = strlen(MyLoraNode.DataTx);
//            UART_write(uart0, &(MyLoraNode.DataTx),MyLoraNode.DataLenTx);
//            UART_write(uart0, "\r\n", 2);
            MyNode.Count ++;
            writeCount(MyNode.Count);
        }else{
            strcpy(MyLoraNode.DataTx,PAYLOAD_9);
            MyLoraNode.DataLenTx = strlen(MyLoraNode.DataTx);
//            UART_write(uart0, &(MyLoraNode.DataTx),MyLoraNode.DataLenTx);
//            UART_write(uart0, "\r\n", 2);
            MyNode.Count = 0;
            writeCount(MyNode.Count);
            MyNode.Iter = 0;
            writeIter(MyNode.Iter);
        }
    }
/*
#ifdef DEBUG
    UART_write(uart0, &(MyLoraNode.DataTx),MyLoraNode.DataLenTx);
    UART_write(uart0, "\r\n", 2);
#endif
*/
    if (MyNode.Mode==MODE_NORMAL_LORA) {
        ret = Tx_Uncnf_Lora(uart1, &MyLoraNode, &mask, &nodeId);
        if (ret==SUCCESS_TX_MAC_TX) {
            MyLoraNode.Upctr = Mac_Get_Upctr(uart1);                            /* Get upctr from RN2483 & Write to file */
            writeUpCntr(MyLoraNode.Upctr);
            MyLoraNode.Dnctr = Mac_Get_Dnctr(uart1);
            writeDnCntr(MyLoraNode.Dnctr);
        } else if (ret == SUCCESS_TX_MAC_RX) {
            MyLoraNode.Upctr = Mac_Get_Upctr(uart1);
            writeUpCntr(MyLoraNode.Upctr);
            MyLoraNode.Dnctr = Mac_Get_Dnctr(uart1);
            writeDnCntr(MyLoraNode.Dnctr);
       /*     if (nodeId==MyNode.NodeId) {
                // Write New Configuration Data to Files
                if (mask&0x01!=0) {
                    writeWakeUp(MyNode.WakeUpInterval);
                }
                if (mask&0x02!=0) {
                    // writeFirstBoot(MyNode.FirstBoot);
                    // writeMode(MyNode.Mode);
                }
                if (mask&0x04!=0) {
                    writeSSID(&(MyNode.SSID));
                }
                if (mask&0x08!=0) {
                    writeNCycles(MyNode.NCycles);
                }
                if (mask&0x10!=0) {
                    writeUpCntr(MyLoraNode.Upctr);
                }
            }*/
            // writeNFails(0);
        } else {
/*
#ifdef DEBUG
            UART_write(uart0,"Tx_Uncnf_Lora() Failed",22);
            UART_write(uart0, "\r\n", 2);
#endif
*/
        }
    }

    else if (MyNode.Mode==MODE_NORMAL_WIFI) {
//        UART_PRINT("SENDING SENSOR DATA OVER MODE_NORMAL_WIFI\r\n");
        prepareDataFrame(PORT, DEST_IP_ADDR);

        sockId = sl_Socket(SL_AF_INET,SL_SOCK_DGRAM, 0);
        if(sockId < 0){
//            UART_PRINT("error UDP %d\n\r",sockId);
        }else{
        }

        status = sl_SendTo(sockId, MyLoraNode.DataTx, MyLoraNode.DataLenTx, 0,(SlSockAddr_t *)&PowerMeasure_CB.ipV4Addr,sizeof(SlSockAddrIn_t));

        if(status < 0 )
        {
            status = sl_Close(sockId);
//            ASSERT_ON_ERROR(sockId);
//            UART_PRINT("\n\rERROR SENDING PACKET: %s\n\r", status);
            Node_Disable();
        }else{
//            UART_PRINT("\n\rPACKET SENT\n\r");
            status = sl_Close(sockId);
//            writeNBoot(1-MyNode.NBoot);       NOT NECCESARY FOR TEST
        }
    }

//    writeNBoot(1-MyNode.NBoot);           NOT NECCESARY FOR TEST
/*
#ifdef DEBUG
    UART_write(uart0, "End of STARPORTS Measures\r\n", 27);
#endif
*/
    UART_close(uart1);
//    UART_close(uart0);                NOT NECCESARY FOR TEST

    Node_Disable();

}

void WakeupFromLPDS(uint_least8_t var) {
    LPDSOut = 1;
}


