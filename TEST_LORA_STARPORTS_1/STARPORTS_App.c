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

// #define MYRTC 1

uint8_t Timer0Event = 0;
uint8_t Timer1Event = 0;
uint8_t LPDSOut = 0;

struct Node MyNode = {NODEID,1200,MODE_NORMAL_LORA,1,16,"PORTS0"};
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

    int upctr;

    /* Timer Handle */
    Timer_Handle timer0;

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

    /* wifi var */
    int16_t         sockId;
    int16_t         status = -1;

    // Quicky enable de node (the jumper an be removed)
    GPIO_setConfig(Board_EN_NODE, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_HIGH);
    Node_Enable();

    // Configure the rest of GPIO pins
    ret = GPIO_Config();

    /************ Begin Read Configuration Files **************************/
    // Muestra estado memoria y lista de archivos
    SPI_init(); //necesario inicializar SPI antes de sl_Start
    sl_Start(0, 0, 0); //necesario para poder trabajar con FS

    uart0 = Startup_UART(Board_UART0, 115200); // arrancado antes de leer para debug

    /* Get Param Values from internal filesystem */
    // Get MyNode.WakeUpInterval --> Read WakeUp_Time File
    MyNode.WakeUpInterval = st_readFileWakeUp();
    // Get MyNode.Mode --> Read File Mode
    MyNode.Mode = st_readFileMode();
    // Get MyNode.NCycles --> Read File Ncycles
    MyNode.NCycles = st_readFileNCycles();
    // Get MyNode.SSID[] --> Read SSID File
//    MyNode.SSID = st_readFileSSID(); //REVISAR, LA LECTURA NO ES CORRECTA
    // Get MyNode.FirstBoot --> Read FirstBoot File: Yes (1) No (0)
    MyNode.FirstBoot = st_readFileFirstBoot();
    // Get MyNode.NFails --> Number of failed attempts to Wireless Connection
    MyNode.NFails=st_readFileNFails();
    /************ Ends Reading Configuration Files **************************/

    if (MyNode.NFails>=4) {
        UART_PRINT("ENTRO EN IF\n\r");
        // Setup Node as WiFi and connect to Known Host
        // ...
        // ...
        MyNode.FirstBoot=TRUE; // and write to file FirstBoot
        writeFirstBoot(MyNode.FirstBoot);
        MyNode.NFails=0; // and write to file NFails
        writeNFails(MyNode.NFails);
    }

    /************* Begin Configure Peripherals ***********************/
    // Reset the RN2483
    RN2483_Clear();
    // I2C interface started
    i2c = Startup_I2C(Board_I2C0);
    // Configures the RTC
    DS1374_Write_Ctrl(i2c);
    // UART0 is for sending debug information */
//    uart0 = Startup_UART(Board_UART0, 115200);
    // UART1 connects to RN2483 */
    uart1 = Startup_UART(Board_UART1, 57600);
    /************* End Configure Peripherals ***********************/

    UART_write(uart0, "\r\nInitiating Test of STARPORTS...\r\n", 35);


    /*************** Begin Setting Node Configuration Parameters */
    // Set the WakeUp_Time in RTC
    DS1374_Clear_AF(i2c);
    DS1374_Write_WdAlmb(i2c, MyNode.WakeUpInterval);

    /************** Begin Configuration and Setup Wireless Connectivity ***************/
    if (MyNode.Mode==MODE_NORMAL_LORA) {
        RN2483_Set();                   // Set /MCLR Pin to 1 releases RN2483
        sz = GetLine_UART(uart1, buf);  // Recoge texto de inicialización del RN2483
        if (sz==0) {
          // Buffer size is too large
        }

        if (MyNode.FirstBoot==TRUE) {
            // Join OTAA
            ret = Join_Otaa_Lora(uart1);
            if (ret==SUCCESS_OTAA_LORA) {
                // Get devaddr from RN2483 & Write to file
                ret = Mac_Get_Devaddr(uart1, &MyLoraNode);
                ret = Mac_Set_Devaddr(uart1, &MyLoraNode);
                // Get upctr from RN2483 & Write to file
                MyLoraNode.Upctr = Mac_Get_Upctr(uart1);
                writeUpCntr(MyLoraNode.Upctr);
                ret = Mac_Save(uart1);
                MyNode.FirstBoot = FALSE; // Write FirstBoot file
                writeFirstBoot(MyNode.FirstBoot);
                MyNode.NFails=0; // and write to file NFails
                writeNFails(MyNode.NFails);
            } else {
                strcpy(Mess,"Join_Otaa_Lora() Failed ");
                UART_write(uart1, Mess, 24);
                Mess[0] = '('; Mess[1] = ret+48; Mess[2]=')';
                UART_write(uart1, Mess,3);
                UART_write(uart1, "\r\n",2);
                MyNode.NFails++; // Write NFails File
                writeNFails(MyNode.NFails);
//***                NextStep=SHUTDOWN;
            }

        } else {
            // Read upctr from file
            // Join ABP
            if (ret==SUCCESS_ABP_LORA) {
                MyNode.NFails=0; // and write to file NFails
                writeNFails(MyNode.NFails);
                // Continue reading sensors
            } else {
                MyNode.NFails++;   // Write NFails File
                writeNFails(MyNode.NFails);
//***                NextStep=SHUTDOWN;
            }
        }
    } else if (MyNode.Mode==MODE_NORMAL_WIFI) {
        if (MyNode.FirstBoot==TRUE) {
            // Read SSID from file
            // Read other params ...
            // Setup WiFi
            // Connect WiFi
            if (ret==SUCCESS_CONNECT_WIFI) {
                MyNode.FirstBoot = FALSE;  // Write FirstBoot File
                writeFirstBoot(MyNode.FirstBoot);
                MyNode.NFails=0; // and write to file NFails
                writeNFails(MyNode.NFails);
            } else {
                MyNode.NFails++;
                writeNFails(MyNode.NFails);
//***                NextStep=SHUTDOWN;
            }
        } else {
            // Read SSID from file
            // Read other params ...
            // Setup WiFi
            // Connect WiFi
            if (ret==SUCCESS_CONNECT_WIFI) {
                MyNode.NFails=0; // and write to file NFails
                // Continue reading sensors
            } else {
                MyNode.NFails++;
                // Write NFails File
//***                NextStep=SHUTDOWN;
            }
        }
    }
    /************** End of Configuration and Setup Wireless Connectivity ***************/

//***    if (NextStep==SHUTDOWN) {
        I2C_close(i2c);
//***        I2C_As_GPIO_Low();  // Puts SCL and SDA signals low to save power
        Node_Disable();     // Auto Shutdown
//***    }

    /************** Begin Reading Data from Sensors ***********************************/
    /* Timer */
    Timer_init();
    /* Configure SPI Master at 5 Mbps, 8-bits, CPOL=0, PHA=0 */
//***    SPI_CS_Disable();   // Put CS to Logic High
    spi = Startup_SPI(Board_SPI_MASTER, 8, 5000000);
    DataPacketLen = GetSensorData(DataPacket);

    /* Close all sensor related peripherals */
    SPI_close(spi);
    // Not really enable the SPI but just puts the CS signal to '0'
    // to prevent PCB from not really shutting down (a high value in some SPI signals
    // keeps sensor chips active)
    LDC1000_SPI_Enable();
    ADXL355_SPI_Enable();
    BME280_SPI_Enable();
//***    SPI_As_GPIO_Low(); // Puts all rest of SPI signals to '0' to save power

    I2C_close(i2c);
//***    I2C_As_GPIO_Low();  // Puts SCL and SDA signals low to save power

    MyLoraNode.DataLenTx = Uint8Array2Char(DataPacket, DataPacketLen, MyLoraNode.DataTx);

    if (MyNode.Mode==MODE_NORMAL_LORA) {
        ret = Tx_Uncnf_Lora(uart1, &MyLoraNode);    // Transmit Data, several tries?
        if (ret==SUCCESS_TX_MAC_TX) {
            // Get upctr from RN2483 & Write to file
            MyLoraNode.Upctr = Mac_Get_Upctr(uart1);
            writeUpCntr(MyLoraNode.Upctr);
            // Getting Configuration Data back from AppServer
            // Write New Configuration Data to Files
            MyNode.NFails=0; // and write to file NFails
            writeNFails(MyNode.NFails);
        } else {
            MyNode.NFails++;
            writeNFails(MyNode.NFails);
        }

    }
/***    else if (MyNode.Mode==MODE_NORMAL_WIFI) {
        // Transmit Data through WiFi, several tries?
        if (ret==SUCCESS_WIFI_TX) {
            // Getting Configuration Data back from AppServer
            // Write New Configuration Data to Files
            MyNode.NFails=0; // and write to file NFails
        } else {
            MyNode.NFails++;
            // Write NFails File
        }
    }
***/
    Node_Disable();

}

void WakeupFromLPDS(uint_least8_t var) {
    LPDSOut = 1;
}


