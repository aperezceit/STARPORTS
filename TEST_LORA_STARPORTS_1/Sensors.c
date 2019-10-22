/*
 * Sensors.c
 *
 *  Created on: 2 may. 2019
 *      Author: airizar
 */

#include <stdlib.h>
#include <ti/drivers/I2C.h>
#include <ti/drivers/UART.h>
#include <ti/drivers/ADC.h>
#include <ti/drivers/SPI.h>
#include <ti/drivers/PWM.h>
#include <ti/drivers/Timer.h>
#include <ti/drivers/Watchdog.h>
#include <unistd.h>
#include "Board.h"
#include "hal_UART.h"
#include "hal_PWM.h"
#include "hal_I2C.h"
#include "hal_Timer.h"
#include "hal_ADC.h"
#include "hal_SPI.h"
#include "hal_GPIO.h"

#include "hal_TMP006.h"
#include "ADXL355.h"
#include "BME280.h"
#include "LDC1000.h"
#include "STARPORTS_App.h"

extern uint8_t Timer1Event;

extern UART_Handle uart0;
extern I2C_Handle i2c;
extern SPI_Handle spi;

extern Watchdog_Handle wd;

extern struct Node MyNode;
extern struct TMP006_Data MyTMP006;
extern struct ADXL355_Data MyADXL;
extern struct BME280_Data MyBME;
extern struct LDC1000_Data MyLDC;
extern struct Vbat_Data MyVbat;

/*!
 *  @brief  Function to initialize the Datapacket to be send by LoRa
 *
 *
 *  @pre    DoIniPacket() has been called
 *
 *  @param  DataPacket    Pointer to the start of a uint8_t array
 *
 *  @param  NodeId        Node Identification (uint16_t)
 *
 *  @return               Returns 1 (the size of the initialize array)
 *
 */
uint8_t IniPacket(uint8_t *DataPacket, uint16_t NodeId) {

    DataPacket[0] = (NodeId & 0xFF00) >> 8;
    DataPacket[1] = (NodeId & 0x00FF);

    return 2;

}

uint8_t Add_s16Data2Packet(uint8_t *DataPacket, uint8_t DataPacketLen, uint16_t SensorId, int16_t *DataSensor, uint8_t DataSensorLen) {

    uint8_t i, j, n, a;

    n=DataPacketLen;

    /* Sensor Info */
    DataPacket[n++] = (SensorId & 0xFF00) >> 8;
    DataPacket[n++] = (SensorId & 0x00FF);
    // DataPacket[n++] = DataSensorLen;
    for (i=0; i<DataSensorLen;i++) {
        for (j=0;j<2;j++) {
            a = (DataSensor[i] >> (8-(j<<3))) & 0x00FF;
            DataPacket[n++] = a;
        }
    }

    /* Return new length of Datapacket */
    return n;

}

uint8_t Add_s32Data2Packet(uint8_t *DataPacket, uint8_t DataPacketLen, uint16_t SensorId, int32_t *DataSensor, uint8_t DataSensorLen) {

    uint8_t i, j, n, a;

    n=DataPacketLen;

    /* Sensor Info */
    DataPacket[n++] = (SensorId & 0xFF00) >> 8;
    DataPacket[n++] = (SensorId & 0x00FF);
    // DataPacket[n++] = DataSensorLen;
    for (i=0; i<DataSensorLen;i++) {
        for (j=0;j<4;j++) {
            a = (DataSensor[i] >> (24-(j<<3))) & 0x00FF;
            DataPacket[n++] = a;
        }
    }

    /* Return new length of Datapacket */
    return n;

}


uint8_t GetSensorData(uint8_t *DataPacket) {

    ADC_Handle adc;
    PWM_Handle pwm;
    Timer_Handle timer;

    int16_t s16DataSensor[32];
    int32_t s32DataSensor[32];
    uint8_t cr;
    uint8_t DataPacketLen;
    uint32_t interval = 10000;

    uint16_t DevId;

    // Variables for Vbat Sensor (ADC)
    int i=0;
    uint16_t adcVal=0;
    uint16_t adcValMean=0;

    // Variables for BME280 Sensor
    uint32_t PressureUn;
    uint32_t TemperatureUn;
    uint16_t HumidityUn;
    struct bme280_calib_data *MyCalib = malloc(sizeof *MyCalib);

    // Variables for LDC1000 Sensor


    // Init Data Packet to Send
    DataPacketLen = IniPacket(DataPacket, MyNode.NodeId);

    if (i2c != NULL) {
        cr = 240;   // 4 conversions per second
        DevId = GetID_TMP006(i2c, Board_I2C_TMP006_ADDR);
        if (DevId==TMP006_ID) {
            SetMOD_TMP006(i2c, Board_I2C_TMP006_ADDR, cr);
            usleep(10000);
            // Get Temp Data and Add Data to Packet
            s16DataSensor[0] = GetTA_TMP006(i2c,Board_I2C_TMP006_ADDR);
            DataPacketLen = Add_s16Data2Packet(DataPacket, DataPacketLen, MyTMP006.SensorId, s16DataSensor, 1);
        }
        // Watchdog_clear(wd);
//        UART_write(uart0, "2 ",2); // Debug Message
    }

    // Get ADC Data (Normally VBat Value)
    // Uses timer to take DataSensorLen[1] values
    // Init Timer to take several measures at exactly the same interval
    // Timer1Event = 0;
    timer = Startup_Oneshot_Timer(Board_TIMER1, interval);
    // Init CC3220 Internal ADC
    adc = Startup_ADC(Board_ADC0);
    if ((adc != NULL) & (timer!=NULL)) {
        do {
            if (Timer_start(timer)==Timer_STATUS_ERROR) break;
            ADC_convert(adc, &adcVal);
            Timer1Event = 0;
            adcValMean += adcVal;
            i++;
            while (!Timer1Event) {;}
        } while (i<=MyVbat.NSamples);
        // Watchdog_clear(wd);
        ADC_close(adc);
        Timer_stop(timer);
        Timer_close(timer);
//        UART_write(uart0, "3 ",2); // Debug Message
        s16DataSensor[0] = (int16_t)(adcValMean/MyVbat.NSamples);
        // Add ADC Data to Packer
        DataPacketLen = Add_s16Data2Packet(DataPacket, DataPacketLen, MyVbat.SensorId, s16DataSensor, 1);
    }

    if (spi != NULL) {
        // ADXL355_Enable();
        // ADXL355_SPI_Enable();
        DevId = ADXL355_DevId(spi);
        if (DevId==ADXL355_ID) {
            ADXL355_Reset(spi);
            ADXL355_Range_Conf(spi, I2C_HIGH_SPEED | RANGE2G);
            ADXL355_Filter_Conf(spi, HPFOFF | ODR250HZ);
            ADXL355_Power_Conf(spi, DRDY_OFF | TEMP_OFF | MEASUREMENT);
            usleep(20000);
            // Get Accelerometer Data
            ADXL355_Get_Accel_Frame(spi, MyADXL.NSamples, s32DataSensor);
            // Add ADXL355 Data to Packet
            DataPacketLen = Add_s32Data2Packet(DataPacket, DataPacketLen, MyADXL.SensorId, s32DataSensor, 6);
        }
        // Watchdog_clear(wd);
        // ADXL355_SPI_Disable();
        // ADXL355_Disable();
//        UART_write(uart0, "4 ",2); // Debug Message

        // BME280_Enable();
        // BME280_SPI_Enable();
        DevId = (uint16_t)BME280_DevId(spi);
        if (DevId==BME280_ID) {
            BME280_Reset(spi);
            usleep(2000);
            BME280_Read_Calib(spi, MyCalib);
            BME280_Write_Ctrl_Hum(spi, OSRS_HX1);
            BME280_Write_Ctrl_Meas(spi, OSRS_TX1 | OSRS_PX1 | FORCED);
            usleep(10000);
            while (BME280_Read_Status(spi)==MEASURING) {}
            PressureUn = BME280_Read_Pressure(spi);
            HumidityUn = BME280_Read_Humidity(spi);
            TemperatureUn = BME280_Read_Temperature(spi);
            s32DataSensor[2] = (int32_t)compensate_temperature(TemperatureUn, MyCalib);
            s32DataSensor[1] = compensate_humidity(HumidityUn, MyCalib);
            s32DataSensor[0] = compensate_pressure(PressureUn, MyCalib);

            // Add BME280 Data to Packet
            DataPacketLen = Add_s32Data2Packet(DataPacket, DataPacketLen, MyBME.SensorId, s32DataSensor, 3);
        }
        free(MyCalib);
        // Watchdog_clear(wd);
        // BME280_SPI_Disable();
        // BME280_Disable();
//        UART_write(uart0, "5 ",2); // Debug Message

        // LDC1000_Enable();
        // LDC1000_SPI_Enable();
        DevId = (uint16_t)LDC1000_DevId(spi);
        if (DevId==LDC1000_ID) {
            pwm = Config_PWM(Board_PWM0);
            PWM_start(pwm); /* Generate CLK Signal of LDC1000 (if necessary) */
            LDC1000_Write_Pow_Conf(spi, STBY_MODE);
            LDC1000_Write_Rp_Max(spi, RPMAX0981);
            LDC1000_Write_Rp_Min(spi, RPMIN0436);
            LDC1000_Write_Min_Freq(spi, 70); // Val = round ( 68.94 x log10(fsensor/2500) )
            LDC1000_Write_Conf(spi, AMP_1V | RESP_TIME_0384);
            LDC1000_Write_Intb_Conf(spi, INTB_DIS);
            LDC1000_Write_Clk_Conf(spi, XIN | CLK_ON);
            LDC1000_Write_Pow_Conf(spi, ACTIVE_MODE);
            LDC1000_Get_Proximity_Frame(spi, MyLDC.NSamples, s32DataSensor);
            PWM_stop(pwm);
            PWM_close(pwm);

            // Add LDC1000 Data to Packet
            DataPacketLen = Add_s32Data2Packet(DataPacket, DataPacketLen, MyLDC.SensorId, s32DataSensor, 2);
        }
        // Watchdog_clear(wd);
        // LDC1000_SPI_Disable();
        // LDC1000_Disable();
//        UART_write(uart0, "6 ",2); // Debug Message
    }

    return DataPacketLen;

}


