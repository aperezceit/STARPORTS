/*
 * hal_LORA.h
 *
 *  Created on: 25 mar. 2019
 *      Author: airizar
 */

#ifndef HAL_LORA_H_
#define HAL_LORA_H_


#include <ti/drivers/UART.h>

#define SUCCESS_ABP_LORA            0
#define ERROR_ABP_DEVADDR           1
#define ERROR_ABP_NWKSKEY           2
#define ERROR_ABP_APPSKEY           3
#define ERROR_ABP_SAVE              4
#define ERROR_ABP_JOIN              5
#define ERROR_ABP_DENIED            6


#define SUCCESS_OTAA_LORA           0
#define ERROR_OTAA_DEVEUI           1
#define ERROR_OTAA_APPEUI           2
#define ERROR_OTAA_APPKEY           3
#define ERROR_OTAA_SAVE             4
#define ERROR_OTAA_JOIN             5
#define ERROR_OTAA_DENIED           6
#define ERROR_OTAA_UNKNOWN          7
#define ERROR_OTAA_INVALID_PARAM    8
#define ERROR_OTAA_KEYS_NOT_INIT    9
#define ERROR_OTAA_NO_FREE_CH       10
#define ERROR_OTAA_SILENT           11
#define ERROR_OTAA_BUSY             12
#define ERROR_OTAA_MAC_PAUSED       13


#define SUCCESS_TX_MAC_TX           0
#define ERROR_TX_MAC_ERR            1
#define ERROR_TX_BUSY               2
#define ERROR_TX_NOT_JOINED         3
#define ERROR_TX_NO_FREE_CH         4
#define ERROR_TX_UNKNOWN            5
#define ERROR_TX_INVALID_DATA_LEN   6
#define ERROR_TX_INVALID_PARAM      7
#define ERROR_TX_SILENT             8
#define ERROR_TX_REJOIN_NEEDED      9
#define ERROR_TX_MAC_PAUSED         10

uint8_t Setup_Abp_Lora(UART_Handle uart, unsigned char *DevEui);

uint8_t Setup_Otaa_Lora(UART_Handle uart, unsigned char *DevEui, unsigned char *AppEui, unsigned char *AppKey);

uint8_t Tx_Uncnf_Lora(UART_Handle uart, unsigned char *DataTx, uint8_t DataLenTx, uint8_t PortNo);

uint8_t Tx_Cnf_Lora(UART_Handle uart, unsigned char *DataTx, uint8_t DataLenTx, unsigned char *DataRx, uint8_t DataLenRx, uint8_t PortNo);

uint8_t Uint8Array2Char(uint8_t *DataPacket, uint8_t DataPacketLen, char *HexStr);

uint8_t IniPacket(uint8_t *DataPacket, uint16_t NodeId);

uint8_t AddDataPacket(uint8_t *DataPacket, uint8_t DataPacketLen, uint16_t SensorId, int16_t *DataSensor, uint8_t DataSensorLen);


#endif /* HAL_LORA_H_ */
