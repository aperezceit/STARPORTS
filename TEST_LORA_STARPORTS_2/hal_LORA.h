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

#define SUCCESS_SET_DEVADDR         0
#define ERROR_SET_DEVADDR           1

#define SUCCESS_SET_UPCTR         0
#define ERROR_SET_UPCTR           1

#define SUCCESS_SET_DNCTR         0
#define ERROR_SET_DNCTR           1

#define SUCCESS_SET_NWSKEY         0
#define ERROR_SET_NWSKEY           1


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
#define ERROR_ADR_SET               14



#define SUCCESS_TX_MAC_TX           0
#define SUCCESS_TX_MAC_RX           11
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


#define SUCCESS_RXDELAY1_SET        0
#define ERROR_RXDELAY1_SET          1

struct LoraNode {
    unsigned char DevEui[16];
    unsigned char DevAddr[8];
    unsigned char AppEui[16];
    unsigned char AppKey[32];
    unsigned char NwksKey[32];
    unsigned char AppsKey[32];
    unsigned char DataTx[128];
    uint8_t DataLenTx;
    uint8_t PortNoTx;
    unsigned char DataRx[4];
    uint8_t DataLenRx;
    uint8_t PortNoRx;
    uint32_t Upctr;
    uint32_t Dnctr;
};

void Reset_RN2483(void);

uint8_t Mac_Get_Deveui(UART_Handle uart, struct LoraNode *MyLoraNode);

uint8_t Mac_Get_Appeui(UART_Handle uart, struct LoraNode *MyLoraNode);

uint8_t Setup_Abp_Lora(UART_Handle uart, struct LoraNode *MyLoraNode);

uint8_t Join_Abp_Lora(UART_Handle uart);

uint8_t Setup_Otaa_Lora(UART_Handle uart, struct LoraNode *MyLoraNode);

uint8_t Join_Otaa_Lora(UART_Handle uart);

uint8_t Mac_Set_Rxdelay1(UART_Handle uart, uint16_t delay);

uint8_t Mac_Save(UART_Handle uart);

uint8_t Mac_Adr_On(UART_Handle uart);

uint8_t Mac_Ar_On(UART_Handle uart);

int Mac_Get_Upctr(UART_Handle uart);

uint8_t Mac_Set_Upctr(UART_Handle uart, struct LoraNode *MyLoraNode);

int Mac_Get_Dnctr(UART_Handle uart);

uint8_t Mac_Set_Dnctr(UART_Handle uart, struct LoraNode *MyLoraNode);

uint8_t Mac_Get_Devaddr(UART_Handle uart, struct LoraNode *MyLoraNode);

uint8_t Mac_Set_Devaddr(UART_Handle uart, struct LoraNode *MyLoraNode);

uint8_t Mac_Clear_Upctr(UART_Handle uart);

uint8_t Try_Join_Lora_Gateway(UART_Handle uart_dbg, UART_Handle uart_lora);

uint8_t Tx_Uncnf_Lora(UART_Handle uart, struct LoraNode *MyLoraNode, uint8_t *mask, uint16_t *nodeId);

uint8_t Tx_Cnf_Lora(UART_Handle uart, struct LoraNode *MyLoraNode, uint8_t *mask, uint16_t *nodeId);

uint8_t Sys_Sleep(UART_Handle uart, uint32_t sleep);

uint8_t Sys_FactoryReset(UART_Handle uart);

void GetLoraRxData(unsigned char *buf, uint8_t size, struct LoraNode *MyLoraNode);

uint8_t Uint8Array2Char(uint8_t *DataPacket, uint8_t DataPacketLen, unsigned char *HexStr);

uint8_t hex2int(unsigned char *hex, uint8_t hlen, uint8_t *bytes);

uint8_t GetLoraServerParams(uint8_t *bytes, uint8_t blen, struct LoraNode *MyLoraNode);

uint8_t IniPacket(uint8_t *DataPacket, uint16_t NodeId);

uint8_t AddDataPacket(uint8_t *DataPacket, uint8_t DataPacketLen, uint16_t SensorId, int16_t *DataSensor, uint8_t DataSensorLen);


#endif /* HAL_LORA_H_ */
