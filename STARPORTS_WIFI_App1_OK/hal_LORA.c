/*
 * hal_LORA.c
 *
 *  Created on: 25 mar. 2019
 *      Author: airizar
 */

#include <string.h>
#include <ti/drivers/UART.h>
#include "Board.h"
#include "hal_LORA.h"
#include "hal_UART.h"


/*!
 *  @brief  Function to setup a lora connection using ABP (Authentification By Personalisation) in a RN2483 based mote
 *          Uses constant values for device address, network key and appskey
 *
 *  @pre    Setup_Abp_Lora() has been called
 *
 *  @param  uart          Logical peripheral number for the UART port connected to the RN2483
 *
 *  @param  DevEUI        Pointer to a char string with the Device EUI returned by RN2483 (output of "sys get hweui" command)
 *
 *  @return               Different SUCCESS/ERROR Conditions according to hal_LORA.h
 *
 */
uint8_t Setup_Abp_Lora(UART_Handle uart, unsigned char *DevEui) {

    unsigned char Command[128];
    unsigned char c, buf[64];
    uint8_t sz;

    strcpy(Command,"sys get hweui\r\n");
    UART_write(uart, (const char *)(Command), strlen((const char *)Command));
    sz = GetLine_UART(uart, buf);
    strncpy(DevEui,buf,16);

    // UART_resetInputBuffer(uart);
    strcpy(Command,"mac set devaddr 89ABCDEF\r\n");
    UART_write(uart, (const char *)Command, strlen((const char *)Command));
    sz = GetLine_UART(uart, buf);
    if (strncmp(buf,"ok",2)!=0) {
        return ERROR_ABP_DEVADDR;
    }

    // UART_resetInputBuffer(uart);
    strcpy(Command,"mac set nwkskey 0123456789ABCDEF0123456789ABCDEF\r\n");
    UART_write(uart, (const char *)Command, strlen((const char *)Command));
    sz = GetLine_UART(uart, buf);
    if (strncmp(buf,"ok",2)!=0) {
        return ERROR_ABP_NWKSKEY;
    }

    // UART_resetInputBuffer(uart);
    strcpy(Command,"mac set appskey 0123456789ABCDEF0123456789ABCDEF\r\n");
    UART_write(uart, (const char *)Command, strlen((const char *)Command));
    sz = GetLine_UART(uart, buf);
    if (strncmp(buf,"ok",2)!=0) {
        return ERROR_ABP_APPSKEY;
    }

    // UART_resetInputBuffer(uart);
    strcpy(Command,"mac save\r\n");
    UART_write(uart, (const char *)Command, strlen((const char *)Command));
    sz = GetLine_UART(uart, buf);
    if (strncmp(buf,"ok",2)!=0) {
        return ERROR_ABP_SAVE;
    }


    // UART_resetInputBuffer(uart);
    strcpy(Command,"mac join abp\r\n");
    UART_write(uart, (const char *)Command, strlen((const char *)Command));
    sz = GetLine_UART(uart, buf);
    if (strncmp(buf,"ok",2)==0) {
        sz = GetLine_UART(uart, buf);
        if (strncmp(buf,"denied",6)==0) {
            return ERROR_ABP_DENIED;
        } else if (strncmp(buf,"accepted",8)==0) {
            return SUCCESS_ABP_LORA;
        } else {
            return ERROR_ABP_JOIN;
        }
    } else {
        return ERROR_ABP_JOIN;
    }

}

/*!
 *  @brief  Function to setup a lora connection using OTAA (Over the Air Activation) in a RN2483 based mote
 *
 *
 *  @pre    Setup_Otaa_Lora() has been called
 *
 *  @param  uart          Logical peripheral number for the UART port connected to the RN2483
 *
 *  @param  DevEUI        Pointer to a char string with the Device EUI returned by RN2483 (output of "sys get hweui" command)
 *
 *  @param  AppEui        Pointer to a char string with the App EUI value for the OTAA link (obtained from the Gateway, 64bits --> 16 chars length)
 *
 *  @param  AppKey        Pointer to a char string with the App Key value for the OTAA link (obtained from the Gateway, 128bits --> 32 chars length)
 *
 *  @return               Different SUCCESS/ERROR Conditions according to hal_LORA.h
 *
 */
uint8_t Setup_Otaa_Lora(UART_Handle uart, unsigned char *DevEui, unsigned char *AppEui, unsigned char *AppKey) {

    unsigned char Command[128];
    unsigned char buf[64];
    uint8_t sz;

    strcpy(Command,"sys get hweui\r\n");
    UART_write(uart, (const char *)(Command), strlen((const char *)Command));
    sz = GetLine_UART(uart, buf);
    strncpy(DevEui,buf,16);

    strcpy(Command,"mac set deveui ");
    strcat(Command, DevEui);
    strcat(Command, "\r\n");
    UART_write(uart, (const char *)Command, strlen((const char *)Command));
    sz = GetLine_UART(uart, buf);
    if (strncmp(buf,"ok",2)!=0) {
        return ERROR_OTAA_DEVEUI;
    }

    strcpy(Command,"mac set appeui ");
    strcat(Command, AppEui);
    strcat(Command, "\r\n");
    UART_write(uart, (const char *)Command, strlen((const char *)Command));
    sz = GetLine_UART(uart, buf);
    if (strncmp(buf,"ok",2)!=0) {
        return ERROR_OTAA_APPEUI;
    }

    strcpy(Command,"mac set appkey ");
    UART_write(uart, (const char *)Command, strlen((const char *)Command));
    strcat(Command, AppKey);
    strcat(Command, "\r\n");
    sz = GetLine_UART(uart, buf);
    if (strncmp(buf,"ok",2)!=0) {
        return ERROR_OTAA_APPKEY;
    }

    strcpy(Command,"mac set adr on\r\n");
    UART_write(uart, (const char *)Command, strlen((const char *)Command));
    sz = GetLine_UART(uart, buf);
    if (strncmp(buf,"ok",2)!=0) {
        return ERROR_OTAA_APPKEY;
    }

    strcpy(Command,"mac save\r\n");
    UART_write(uart, (const char *)Command, strlen((const char *)Command));
    sz = GetLine_UART(uart, buf);
    if (strncmp(buf,"ok",2)!=0) {
        return ERROR_OTAA_SAVE;
    }

    strcpy(Command,"mac join otaa\r\n");
    UART_write(uart, (const char *)Command, strlen((const char *)Command));
    sz = GetLine_UART(uart, buf);
    if (strncmp(buf,"ok",2)==0) {
        sz = GetLine_UART(uart, buf);
        if (strncmp(buf,"denied",6)==0) {
            return ERROR_OTAA_DENIED;
        } else if (strncmp(buf,"accepted",8)==0) {
            return SUCCESS_OTAA_LORA;
        } else {
            return ERROR_OTAA_UNKNOWN;
        }
    } else if (strncmp(buf,"invalid_param",13)==0) {
        return ERROR_OTAA_INVALID_PARAM;
    } else if (strncmp(buf,"keys_not_init",13)==0) {
        return ERROR_OTAA_KEYS_NOT_INIT;
    } else if (strncmp(buf,"no_free_ch",10)==0) {
        return ERROR_OTAA_NO_FREE_CH;
    } else if (strncmp(buf,"silent",6)==0) {
        return ERROR_OTAA_SILENT;
    } else if (strncmp(buf,"busy",4)==0) {
        return ERROR_OTAA_BUSY;
    } else if (strncmp(buf,"mac_paused",10)==0) {
        return ERROR_OTAA_MAC_PAUSED;
    } else {
        return ERROR_OTAA_UNKNOWN;
    }

}

/*!
 *  @brief  Function to transmit data using lora connection in a RN2483 based mote in unconfirmed mode
 *
 *
 *  @pre    Tx_Uncnf_Lora() has been called
 *
 *  @param  uart          Logical peripheral number for the UART port connected to the RN2483 using command "mac tx uncnf <PortNo> <DataTx>"
 *
 *  @param  DataTx        Pointer to a char string with the DataTx string (see above)
 *
 *  @param  DataLenTx     Length of DataTx string in chars
 *
 *  @param  PortNo        Port Number (see above), must be between 1-223
 *
 *  @return               Different SUCCESS/ERROR Conditions according to hal_LORA.h
 *
 */
uint8_t Tx_Uncnf_Lora(UART_Handle uart, unsigned char *DataTx, uint8_t DataLenTx, uint8_t PortNo) {

    unsigned char Command[256];
    unsigned char buf[40];
    uint8_t sz;


    sprintf(Command,"mac tx uncnf %d ",PortNo);
    strncat(Command,DataTx,DataLenTx);
    strcat(Command,"\r\n");

    UART_write(uart, Command, strlen(Command));
    sz = GetLine_UART(uart, buf);

    if (strncmp(buf,"ok",2)==0) {
        sz = GetLine_UART(uart, buf);
        if (strncmp(buf,"mac_tx_ok",9)==0) {
            return SUCCESS_TX_MAC_TX;
        } else if (strncmp(buf,"mac_err",9)==0) {
            return ERROR_TX_MAC_ERR;
        } else if (strncmp(buf,"invalid_data_len",9)==0) {
            return ERROR_TX_INVALID_DATA_LEN;
        } else {
            return ERROR_TX_UNKNOWN;
        }
    } else if (strncmp(buf,"invalid_param",13)==0) {
        return ERROR_TX_INVALID_PARAM;
    } else if (strncmp(buf,"not_joined",13)==0) {
        return ERROR_TX_NOT_JOINED;
    } else if (strncmp(buf,"no_free_ch",10)==0) {
        return ERROR_TX_NO_FREE_CH;
    } else if (strncmp(buf,"silent",6)==0) {
        return ERROR_TX_SILENT;
    } else if (strncmp(buf,"frame_counter_err_rejoin_needed",31)==0) {
        return ERROR_TX_REJOIN_NEEDED;
    } else if (strncmp(buf,"busy",4)==0) {
        return ERROR_TX_BUSY;
    } else if (strncmp(buf,"mac_paused",10)==0) {
        return ERROR_TX_MAC_PAUSED;
    } else if (strncmp(buf,"invalid_data_len",16)==0) {
        return ERROR_TX_INVALID_DATA_LEN;
    } else {
        return ERROR_TX_UNKNOWN;
    }

}

/*!
 *  @brief  Function to transmit data using lora connection in a RN2483 based mote in confirmed mode
 *
 *
 *  @pre    Tx_Cnf_Lora() has been called
 *
 *  @param  uart          Logical peripheral number for the UART port connected to the RN2483 using command "mac tx cnf <PortNo> <DataTx>"
 *
 *  @param  DataTx        Pointer to a char string with the DataTx string (see above)
 *
 *  @param  DataLenTx     Length of DataTx string in chars
 *
 *  @param  DataRx        Pointer to a char string with the DataRx string returned by "mac tx cnf " --> mac_rx <portno> <data>
 *
 *  @param  DataLenRx     Length of the DataRx string
 *
 *  @param  PortNo        Port Number (see above), must be between 1-223
 *
 *  @return               Different SUCCESS/ERROR Conditions according to hal_LORA.h
 *
 */
uint8_t Tx_Cnf_Lora(UART_Handle uart, unsigned char *DataTx, uint8_t DataLenTx, unsigned char *DataRx, uint8_t DataLenRx, uint8_t PortNo) {

    unsigned char Command[256];
    unsigned char buf[32];
    uint8_t sz;

    sprintf(Command,"mac tx cnf %d ",PortNo);
    strncat(Command,DataTx,DataLenTx);
    strcat(Command,"\r\n");

    UART_write(uart, Command, strlen(Command));
    sz = GetLine_UART(uart, buf);

    if (strncmp(buf,"ok",2)==0) {
        sz = GetLine_UART(uart, buf);
        // Get Acknowledge Data
        if (strncmp(buf,"mac_err",9)==0) {
            return ERROR_TX_MAC_ERR;
        } else if (strncmp(buf,"invalid_data_len",9)==0) {
            return ERROR_TX_INVALID_DATA_LEN;
        } else if (strncmp(buf,"mac_rx",6)==0) {
            sz = GetLine_UART(uart, buf);
            // Get Acknowledge Data
            if (strncmp(buf,"mac_rx",6)==0) {
                sz = GetLine_UART(uart, buf);
                if (strncmp(buf,"mac_tx_ok",9)==0) {
                    return SUCCESS_TX_MAC_TX;
                } else {
                    return ERROR_TX_UNKNOWN;
                }
            } else {
                return ERROR_TX_UNKNOWN;
            }
        } else {
            return ERROR_TX_UNKNOWN;
        }
    } else if (strncmp(buf,"invalid_param",13)==0) {
        return ERROR_TX_INVALID_PARAM;
    } else if (strncmp(buf,"not_joined",13)==0) {
        return ERROR_TX_NOT_JOINED;
    } else if (strncmp(buf,"no_free_ch",10)==0) {
        return ERROR_TX_NO_FREE_CH;
    } else if (strncmp(buf,"silent",6)==0) {
        return ERROR_TX_SILENT;
    } else if (strncmp(buf,"frame_counter_err_rejoin_needed",31)==0) {
        return ERROR_TX_REJOIN_NEEDED;
    } else if (strncmp(buf,"busy",4)==0) {
        return ERROR_TX_BUSY;
    } else if (strncmp(buf,"mac_paused",10)==0) {
        return ERROR_TX_MAC_PAUSED;
    } else if (strncmp(buf,"invalid_data_len",16)==0) {
        return ERROR_TX_INVALID_DATA_LEN;
    } else {
        return ERROR_TX_UNKNOWN;
    }

}


uint8_t Uint8Array2Char(uint8_t *DataPacket, uint8_t DataPacketLen, char *HexStr) {

    uint8_t i, j, k;
    uint16_t a;

    for (i=0;i<DataPacketLen;i++) {
        for (j=0;j<2;j++) {
            a = (DataPacket[i] >> (4-(j<<2))) & 0x0f;
            k = (i<<1)+j;
            HexStr[k] = (a<10)? a+48 : a+55;
        }
    }
    HexStr[k+1] = '\0';

    return k+1;

}


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

    (*DataPacket) = NodeId;

    return 1;

}

uint8_t AddDataPacket(uint8_t *DataPacket, uint8_t DataPacketLen, uint16_t SensorId, int16_t *DataSensor, uint8_t DataSensorLen) {

    uint8_t i, j, n, a;

    n=DataPacketLen;

    /* Sensor Info */
    DataPacket[n++] = (SensorId & 0xFF00) >> 8;
    DataPacket[n++] = (SensorId & 0x00FF);
    DataPacket[n++] = DataSensorLen;
    for (i=0; i<DataSensorLen;i++) {
        for (j=0;j<2;j++) {
            a = (DataSensor[(i<<1)+j] >> (8-(j<<3))) & 0x00FF;
            DataPacket[n++] = a;
        }
    }

    /* Return new length of Datapacket */
    return n;

}
