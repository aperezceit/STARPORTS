/*
 * STARPORTS_App.h
 *
 *  Created on: 15 abr. 2019
 *      Author: airizar
 */

#ifndef STARPORTS_APP_H_
#define STARPORTS_APP_H_

#include <stdint.h>

#define NODEID              0x7abc
#define ADXL355_ID          0xAD1D
#define BME280_ID           0x0060
#define LDC1000_ID          0x0084
#define VBAT_ID             0x0055
#define TMP006_ID           0x5449

#define MODE_NORMAL_LORA        0x00
#define MODE_STORAGE_LORA       0x01
#define MODE_NORMAL_WIFI        0x02
#define MODE_STORAGE_WIFI       0x03
#define MODE_AP_WIFI            0x04


#define SHUTDOWN    1
#define CONTINUE    0

#define RNW_LSB     0
#define RNW_MSB     1

#define NUMBER_SENSORS  6


////////////////////////
/////LORA KEYS/////////
///////////////////////

#define DEVEUI      "0004A30B00EB15E8"
#define APPEUI      "70B3D57ED0019CD5"
#define APPKEY      "E506C0086DA5C31C301FCAC72BC808F6"
#define DEVADDR     "2601267B"
#define NWKSKEY     "1AE937D87A65EDE42B1FCE61BCF277B9"
#define APPSKEY     "56297CFF77806B54D335DE9F94745DEB"

uint8_t NodeState;

typedef enum {
    POWERUP,
    GET_PARAMS,
    CONFIG_LORA,
    CONFIG_WIFI,
    GET_SENSOR_DATA,
    SEND_DATA_LORA,
    SEND_DATA_WIFI,
    STORE_DATA,
    GET_NEW_PARAMS,
    STORE_NEW_PARAMS
} Node_states_t;

struct Node {
    uint16_t NodeId;
    uint16_t WakeUpInterval;
    uint8_t Mode;
    uint16_t NCycles;
    unsigned char SSID[13];
    uint16_t NFails;
    uint8_t FirstBoot;
    uint8_t NSensors;
    uint16_t *SensorIdPtr;
    uint8_t *SensorDataLen;
    int16_t **SensorDataPtr;
};


struct TMP006_Data {
    uint16_t SensorId;      // Identificador del Sensor
};

struct ADXL355_Data {
    uint16_t SensorId;      // Identificador del Sensor
    uint16_t SampleRate;            // Frecuencia de Muestreo (Hz)
    uint16_t NSamples;              // Numero de Muestras a procesar
};

struct BME280_Data {
    uint16_t SensorId;
};

struct LDC1000_Data {
    uint16_t SensorId;       // Identificador del Sensor
    uint16_t NSamples;              // Number of Samples to Process
};

struct Vbat_Data {
    uint16_t SensorId;              // Identificador del Sensor
    uint16_t NSamples;              // Number of Samples to Process
};

struct Keller_Data {
    uint16_t SensorId;              // Identificador del Sensor
};



//    int32_t AxMean;         // Acc Media de NSamples en Eje-X
//    int32_t AyMean;         // Acc Media de NSamples en Eje-Y
//    int32_t AzMean;         // Acc Media de NSamples en Eje-Z
//    int32_t AxRms;          // Acc RMS de NSamples en Eje-X
//    int32_t AyRms;          // Acc RMS de NSamples en Eje-Y
//    int32_t AzRms;          // Acc RMS de NSamples en Eje-Z
//};



#endif /* STARPORTS_APP_H_ */
