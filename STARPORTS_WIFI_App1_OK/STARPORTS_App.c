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
// #include <ti/drivers/GPIO.h>
#include <ti/drivers/UART.h>
#include <ti/drivers/I2C.h>
#include <ti/drivers/GPIO.h>
#include <ti/drivers/PWM.h>
#include <ti/drivers/SPI.h>
#include <ti/drivers/ADC.h>
#include <ti/drivers/Timer.h>
#include <ti/drivers/net/wifi/wlan.h>
#include <ti/drivers/net/wifi/simplelink.h>


#include "hal_PWM.h"
#include "hal_UART.h"
#include "hal_I2C.h"
#include "hal_SPI.h"
#include "hal_LORA.h"
#include "hal_ADC.h"
#include "hal_Timer.h"
#include "hal_TMP006.h"
#include "wifi.h"

/* Example/Board Header files */
#include "Board.h"

#include "STARPORTS_App.h"


/*
 *  ======== mainThread ========
 */

void *mainThread(void *arg0)
{
    /* UART Handles */
    UART_Handle uart0, uart1;

    /* I2C Handles */
    I2C_Handle i2c;

    /*SPI Handle */
    SPI_Handle masterspi;

    /* PWM Handles */
    PWM_Handle pwm = NULL;

    /* ADC Handle */
    ADC_Handle adc;

    uint8_t PortNo = 1;

    unsigned char DevEui[16];
    unsigned char Mess[32];

    uint32_t NodeId = 0x7abc;

    uint8_t DataPacket[256];
//    char* DataPacket[256];
    uint8_t DataPacketLen;

    int16_t DataSensor0[64];
    uint8_t DataSensor0Len;
    uint16_t Sensor0Id = 0x4321;

    int16_t DataSensor1[64];
    uint8_t DataSensor1Len;
    uint16_t Sensor1Id = 0x1234;

    char* buf[256];
    uint8_t DataBufLen;

    uint8_t ret;
    uint8_t cr;
    uint8_t sz;
    uint16_t val;

    /* wifi var */
    int16_t         sockId;
    int16_t         status = -1;
//    char*    buffer = buf;

    Timer_Handle timer;
    Timer_Params params;

    /* Timer */
    timer = Startup_Timer(Board_TIMER0, 100000);

    /* ADC */
    adc = Startup_ADC(Board_ADC0);

    /* Generate CLK Signal of LDC1000 */
    pwm = Startup_PWM(Board_PWM0);
    PWM_start(pwm);

    /* UART0 is for sending debug information */
    uart0 = Startup_UART(Board_UART0, 115200);
    /* UART1 connects to RN2483 */
//    uart1 = Startup_UART(Board_UART1, 57600);
//    UART_PRINT("UART 1 CONNECTED TO RN2483! \r\n");

    /* I2C for Temp Sensor */
    i2c = Startup_I2C(Board_I2C0);
    cr = 240;   // 4 conversions per second
    SetMOD_TMP006(i2c, Board_I2C_TMP006_ADDR, cr);
    UART_PRINT("I2C OK! \r\n");
    usleep(1000000);
    /* ------------------------ */

    /* SPI */
    // Configure SPI Master at 1 Mbps, 8-bits, CPOL=0, PHA=1
    masterspi = Startup_SPI(Board_SPI_MASTER, 1000000);
    UART_PRINT("SPI OK! \r\n");
    usleep(1000000);

    // /MCLR Pin Configured, and set at Low
    GPIO_setConfig(Board_RN2483_MCLR, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);

    // GPIOs Configured and set as outputs in Low state
    GPIO_setConfig(Board_GPIO6,     GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);
    // GPIOs Configured and set as outputs in High state
    GPIO_setConfig(Board_GPIO8,     GPIO_CFG_OUT_STD | GPIO_CFG_OUT_HIGH);
    // GPIO_setConfig(Board_GPIO7,     GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);
    // GPIO_setConfig(Board_GPIO8,     GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);

    // INTB Inputs from LDC1000 & DS1374
    // GPIO_setConfig(Board_GPIO7,     GPIO_CFG_IN_NOPULL );
    // GPIO_setConfig(Board_GPIO8,     GPIO_CFG_IN_NOPULL );

    // Release /MCLR Pin
    GPIO_write(Board_RN2483_MCLR, 1);
    GPIO_write(Board_GPIO8, 0);
    usleep(2000000);
    UART_PRINT("GPIO OK! \r\n");

    /* ----------------------------- */

    // Recoge texto de inicialización del RN2483
/*    sz = GetLine_UART(uart1, buf);
    if (sz==0) {
        // Buffer size is too large
    }
*/
    /* Configures LoRa using ABP */
/*    ret = Setup_Abp_Lora(uart1, DevEui);
    if (ret==SUCCESS_ABP_LORA) {
        strcpy(Mess,"Setup_Abp_Lora() Success\r\n");
        UART_write(uart0, Mess, strlen(Mess));
        UART_PRINT("LORA SETUP SUCCESS %s\n\r");
    } else {
        strcpy(Mess,"Setup_Abp_Lora() Failed ");
        UART_PRINT("LORA SETUP FAILED %s\n\r");
        UART_write(uart0, Mess, strlen(Mess));
        Mess[0] = '('; Mess[1] = ret+48; Mess[2]=')';
        UART_write(uart0, Mess,3);
        UART_write(uart0, "\r\n",2);
    }
*/
    /* Test SPI */
    uint8_t Data;
    ret = SPI_write_8bits(masterspi, 0x01, 0xAA);
    ret = SPI_read_8bits(masterspi, 12, &Data);
    ret = SPI_read_8bits(masterspi, 12+16, &Data);
    ret = SPI_read_8bits(masterspi, 12+32, &Data);
    ret = SPI_read_8bits(masterspi, 12+48, &Data);
    ret = SPI_read_8bits(masterspi, 12+64, &Data);
    ret = SPI_read_8bits(masterspi, 12+80, &Data);
    ret = SPI_read_8bits(masterspi, 12+96, &Data);
    ret = SPI_read_8bits(masterspi, 12+112, &Data);
    /* End of SPI Test */
    UART_PRINT("TEST SPI OK! \r\n");
    usleep(100000);

    /* WIFI */
    wlanConf();
    wlanConnect();
    UART_PRINT("CONNECTED TO AP! \r\n");
    usleep(1000000);
    startTcpServer();

    prepareDataFrame(PORT, DEST_IP_ADDR);

    sockId = sl_Socket(SL_AF_INET,SL_SOCK_STREAM, 0);
    ASSERT_ON_ERROR(sockId);

    status = sl_Connect(sockId, ( SlSockAddr_t *)&PowerMeasure_CB.ipV4Addr, sizeof(SlSockAddrIn_t));
    if(status < 0)
    {
        sl_Close(sockId);
        ASSERT_ON_ERROR(sockId);
        UART_PRINT("CLIENT REFUSED %s\n\r", ( SlSockAddr_t *)&PowerMeasure_CB.ipV4Addr);
    }
    else{
        UART_PRINT("CLIENT CONNECTED %s\n\r",( SlSockAddr_t *)&PowerMeasure_CB.ipV4Addr);
        usleep(100000);
    }

    Timer_start(timer);
    // Read Data From Sensor
    while(1) {
        DataSensor0[0] = GetTA_TMP006(i2c,Board_I2C_TMP006_ADDR);
        DataSensor0Len = 1;
        ADC_convert(adc, (uint16_t *)DataSensor1);
        DataSensor1Len = 1;

        DataPacketLen = IniPacket(DataPacket, NodeId);
        DataPacketLen = AddDataPacket(DataPacket, DataPacketLen, Sensor0Id, DataSensor0, DataSensor0Len);
        DataPacketLen = AddDataPacket(DataPacket, DataPacketLen, Sensor1Id, DataSensor1, DataSensor1Len);
        DataBufLen = Uint8Array2Char(DataPacket, DataPacketLen, buf);

        ltoa(DataSensor1[0],buf);
/*        UART_write(uart0, buf, strlen(buf));
        UART_write(uart0,"\r\n",2);
*/
        /* Transmit Data through LoRa */
        // Convert Data Array in Character String
/*
        ret = Tx_Uncnf_Lora(uart1, buf, DataBufLen, PortNo);
        if (ret==SUCCESS_TX_MAC_TX) {
            strcpy(Mess,"Tx_Uncnf_Lora() Success\r\n");
            UART_write(uart0, Mess, strlen(Mess));
            UART_PRINT("LORA TX SUCCESS %s\n\r");
        } else {
            strcpy(Mess,"Tx_Uncnf_Lora() Failed ");
            UART_write(uart0, Mess, strlen(Mess));
            UART_PRINT("LORA TX FAILED %s\n\r");
            Mess[0] = '('; Mess[1] = ret+48; Mess[2]=')';
            UART_write(uart0, Mess,3);
            UART_write(uart0, "\r\n",2);
        }
*/      usleep(1000000);

        /* Transmit Data through wifi */
        status = sl_Send(sockId, buf, sizeof(buf), 0 );
        if(status < 0 )
        {
            status = sl_Close(sockId);
            ASSERT_ON_ERROR(status);
            UART_PRINT("ERROR SENDING PACKET: %s\n\r", status);
        }
        else if(status == 0 )
        {
            ASSERT_ON_ERROR(status);
            UART_PRINT("PACKET SENT SUCCESSFULLY: %s\n\r", status);
            status = sl_Close(sockId);
            UART_PRINT("CLIENT DISCONNECTED %s\n\r");
        }
        else
        {
            UART_PRINT("PACKET SENT SUCCESSFULLY: %s\n\r", buf);
        }
        usleep(1000000);
    }

    return (0);
}

