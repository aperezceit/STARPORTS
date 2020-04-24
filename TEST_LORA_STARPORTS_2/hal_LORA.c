/*
 * hal_LORA.c
 *
 *  Created on: 25 mar. 2019
 *      Author: airizar
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <ti/drivers/UART.h>
#include <ti/drivers/GPIO.h>
#include "Board.h"
#include "hal_LORA.h"
#include "hal_UART.h"
#include "file_system.h"
#include "STARPORTS_App.h"

#include <ti/drivers/net/wifi/simplelink.h>


extern UART_Handle uart0;
extern struct Node MyNode;

extern struct ADXL355_Data MyADXL;
extern struct BME280_Data MyBME;
extern struct LDC1000_Data MyLDC;

void Reset_RN2483(void) {

    // Activate /MCLR Pin
    GPIO_write(Board_RN2483_MCLR, 0);
    usleep(50000);
    // Release /MCLR Pin
    GPIO_write(Board_RN2483_MCLR, 1);

}

void Factory_reset(UART_Handle uart){

    unsigned char Command[128];
    unsigned char buf[64];
    uint8_t sz;

    strcpy(Command,"sys factoryRESET\r\n");
    UART_write(uart, (const char *)(Command), 18);

}

uint8_t Mac_Get_Deveui(UART_Handle uart, struct LoraNode *MyLoraNode) {
    unsigned char Command[128];
    unsigned char buf[64];
    uint8_t sz;

    memset(&buf,0, sizeof(buf));
    strcpy(Command,"mac get deveui\r\n");
    UART_write(uart, (const char *)(Command), 16);
    sz = GetLine_UART(uart, buf);
    strncpy(MyLoraNode->DevEui,buf,16);

    return 0;
}

uint8_t Mac_Get_Appeui(UART_Handle uart, struct LoraNode *MyLoraNode) {
    unsigned char Command[128];
    unsigned char buf[64];
    uint8_t sz;

    memset(&buf,0, sizeof(buf));
    strcpy(Command,"mac get appeui\r\n");
    UART_write(uart, (const char *)(Command), 16);
    sz = GetLine_UART(uart, buf);
    strncpy(MyLoraNode->AppEui,buf,16);

    return 0;
}


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
uint8_t Setup_Abp_Lora(UART_Handle uart, struct LoraNode *MyLoraNode) {

    unsigned char Command[128];
    unsigned char c, buf[64];
    uint8_t sz;

    memset(&buf,0, sizeof(buf));

    strcpy(Command,"mac set devaddr ");
    strncat(Command, MyLoraNode->DevAddr,8);
    strcat(Command,"\r\n");
    UART_write(uart, (const char *)Command, 26);
    sz = GetLine_UART(uart, buf);
    if (strncmp(buf,"ok",2)!=0) {
        return ERROR_ABP_DEVADDR;
    }

    memset(&buf,0, sizeof(buf));

    // UART_resetInputBuffer(uart);
    strcpy(Command,"mac set nwkskey ");
    strncat(Command, MyLoraNode->NwksKey,32);
    strcat(Command,"\r\n");
    UART_write(uart, (const char *)Command, 50);
    sz = GetLine_UART(uart, buf);
    if (strncmp(buf,"ok",2)!=0) {
        return ERROR_ABP_NWKSKEY;
    }

    memset(&buf,0, sizeof(buf));
    // UART_resetInputBuffer(uart);
    strcpy(Command,"mac set appskey ");
    strncat(Command, MyLoraNode->AppsKey,32);
    strcat(Command,"\r\n");
    UART_write(uart, (const char *)Command, 50);
    sz = GetLine_UART(uart, buf);
    if (strncmp(buf,"ok",2)!=0) {
        return ERROR_ABP_APPSKEY;
    }

    return SUCCESS_ABP_LORA;

}

uint8_t Join_Abp_Lora(UART_Handle uart) {

    unsigned char Command[128];
    unsigned char buf[64];
    uint8_t sz;

    // UART_resetInputBuffer(uart);
    strcpy(Command,"mac join abp\r\n");
    UART_write(uart, (const char *)Command, 14);
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
uint8_t Setup_Otaa_Lora(UART_Handle uart, struct LoraNode *MyLoraNode) {

    unsigned char Command[128];
    unsigned char buf[64];
    uint8_t sz;

    memset(&buf,0, sizeof(buf));
    strcpy(Command,"sys get hweui\r\n");
    UART_write(uart, (const char *)(Command), 15);
    sz = GetLine_UART(uart, buf);
    strncpy(MyLoraNode->DevEui,buf,16);

    memset(&buf,0, sizeof(buf));
    memset(&Command,0, sizeof(Command));
    strcpy(Command,"mac set deveui ");
    strncat(Command, MyLoraNode->DevEui,16);
    strcat(Command, "\r\n");
    UART_write(uart, (const char *)Command, 33);
    sz = GetLine_UART(uart, buf);
    if (strncmp(buf,"ok",2)!=0) {
        return ERROR_OTAA_DEVEUI;
    }

    memset(&buf,0, sizeof(buf));
    strcpy(Command,"mac set appeui ");
    strncat(Command, MyLoraNode->AppEui,16);
    strcat(Command, "\r\n");
    UART_write(uart, (const char *)Command, 33);
    sz = GetLine_UART(uart, buf);
    if (strncmp(buf,"ok",2)!=0) {
        return ERROR_OTAA_APPEUI;
    }
    else
    {
//        UART_write(uart0, MyLoraNode->AppEui, 16);
    }

    memset(&buf,0, sizeof(buf));
    strcpy(Command,"mac set appkey ");
    strncat(Command, MyLoraNode->AppKey,32);
    strcat(Command, "\r\n");
    UART_write(uart, (const char *)Command, 49);
    sz = GetLine_UART(uart, buf);
    if (strncmp(buf,"ok",2)!=0) {
        return ERROR_OTAA_APPKEY;
    }

    return SUCCESS_OTAA_LORA;
}


uint8_t Join_Otaa_Lora(UART_Handle uart) {

    unsigned char Command[128];
    unsigned char buf[64];
    uint8_t sz;

    memset(&buf,0, sizeof(buf));

    Mac_Adr_On(uart);

    strcpy(Command,"mac join otaa\r\n");
    UART_write(uart, (const char *)Command, 15);
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

uint8_t Mac_Set_Rxdelay1(UART_Handle uart, uint16_t delay) {
    unsigned char Command[128];
    unsigned char buf[64];
    uint8_t sz;

    memset(&buf,0, sizeof(buf));
    sprintf(Command,"mac set rxdelay1 %d\r\n", delay);
    UART_write(uart, (const char *)Command, strlen(Command));
    sz = GetLine_UART(uart, buf);
    if (strncmp(buf,"ok",2)!=0) {
        return ERROR_RXDELAY1_SET;
    } else {
       return  SUCCESS_RXDELAY1_SET;
    }

}


uint8_t Mac_Save(UART_Handle uart) {

    unsigned char Command[256];
    unsigned char buf[32];
    uint8_t sz;

    strcpy(Command,"mac save\r\n");

    UART_write(uart, Command, 10);
    sz = GetLine_UART(uart, buf);

    if (strncmp(buf,"ok",2)==0) {
        return 0;
    } else if (strncmp(buf,"invalid_param",13)==0) {
        return 1;
    } else {
        return 1;
    }

}

uint8_t Mac_Adr_On(UART_Handle uart) {

    unsigned char Command[256];
    unsigned char buf[32];
    uint8_t sz;

    memset(&buf,0, sizeof(buf));
    strcpy(Command,"mac set adr on\r\n");
    UART_write(uart, (const char *)Command, 16);
    sz = GetLine_UART(uart, buf);
    if (strncmp(buf,"ok",2)==0) {
        return 0;
    } else {
        return 1;
    }

}

uint8_t Mac_Ar_On(UART_Handle uart) {

    unsigned char Command[256];
    unsigned char buf[32];
    uint8_t sz;

    memset(&buf,0, sizeof(buf));
    strcpy(Command,"mac set ar on\r\n");
    UART_write(uart, (const char *)Command, 15);
    sz = GetLine_UART(uart, buf);
    if (strncmp(buf,"ok",2)==0) {
        return 0;
    } else {
        return 1;
    }

}

uint8_t Mac_Get_Devaddr(UART_Handle uart, struct LoraNode *MyLoraNode) {
    unsigned char Command[256];
    unsigned char buf[32];
    uint8_t sz;

    strcpy(Command,"mac get devaddr\r\n");
    UART_write(uart, (const char *)(Command), 17);
    sz = GetLine_UART(uart, buf);
    strncpy(MyLoraNode->DevAddr,buf,8);

    return 0;

}

uint8_t Mac_Set_Devaddr(UART_Handle uart, struct LoraNode *MyLoraNode) {
    unsigned char Command[256];
    unsigned char buf[32];
    uint8_t sz;

    memset(&buf,0, sizeof(buf));
    strcpy(Command,"mac set devaddr ");
    strncat(Command, MyLoraNode->DevAddr,sizeof(MyLoraNode->DevAddr));
    strcat(Command,"\r\n");
    UART_write(uart, (const char *)Command, 26);
    sz = GetLine_UART(uart, buf);
    if (strncmp(buf,"ok",2)!=0) {
        return ERROR_SET_DEVADDR;
    } else {
        return SUCCESS_SET_DEVADDR;
    }

}

int Mac_Get_Upctr(UART_Handle uart) {

    unsigned char Command[256];
    unsigned char buf[32];
    uint8_t sz;

    strcpy(Command,"mac get upctr\r\n");
    UART_write(uart, (const char *)(Command), 15);
    sz = GetLine_UART(uart, buf);

    return atoi(buf);

}

uint8_t Mac_Set_Upctr(UART_Handle uart, struct LoraNode *MyLoraNode) {
    unsigned char Command[256];
    unsigned char buf[32];
    uint8_t sz;

    memset(&buf,0, sizeof(buf));
    sprintf(Command,"mac set upctr %d\r\n", MyLoraNode->Upctr);
    UART_write(uart, (const char *)Command, strlen(Command));
    sz = GetLine_UART(uart, buf);
    if (strncmp(buf,"ok",2)!=0) {
        return ERROR_SET_UPCTR;
    } else {
        return SUCCESS_SET_UPCTR;
    }

}

int Mac_Get_Dnctr(UART_Handle uart) {

    unsigned char Command[256];
    unsigned char buf[32];
    uint8_t sz;

    strcpy(Command,"mac get dnctr\r\n");
    UART_write(uart, (const char *)(Command), 15);
    sz = GetLine_UART(uart, buf);

    return atoi(buf);

}

uint8_t Mac_Set_Dnctr(UART_Handle uart, struct LoraNode *MyLoraNode) {
    unsigned char Command[256];
    unsigned char buf[32];
    uint8_t sz;

    memset(&buf,0, sizeof(buf));
    sprintf(Command,"mac set dnctr %d\r\n", MyLoraNode->Dnctr);
    UART_write(uart, (const char *)Command, strlen(Command));
    sz = GetLine_UART(uart, buf);
    if (strncmp(buf,"ok",2)!=0) {
        return ERROR_SET_DNCTR;
    } else {
        return SUCCESS_SET_DNCTR;
    }

}



uint8_t Mac_Clear_Upctr(UART_Handle uart) {

    unsigned char Command[256];
    unsigned char buf[32];
    uint8_t sz;

    strcpy(Command,"mac set upctr 0\r\n");
    UART_write(uart, (const char *)(Command), 17);
    sz = GetLine_UART(uart, buf);
    if (strncmp(buf,"ok",2)==0) {
        return 0;
    } else {
        return 1;
    }
}

uint8_t Mac_Set_Pwridx(UART_Handle uart, uint8_t pwridx) {
    unsigned char Command[256];
    unsigned char buf[32];
    uint8_t sz;

    memset(&buf,0, sizeof(buf));
    sprintf(Command,"mac set pwridx %d\r\n", pwridx);
    UART_write(uart, (const char *)Command, strlen(Command));
    sz = GetLine_UART(uart, buf);
    if (strncmp(buf,"ok",2)!=0) {
        return ERROR_SET_PWRIDX;
    } else {
        return SUCCESS_SET_PWRIDX;
    }

}

uint8_t Try_Join_Lora_Gateway(UART_Handle uart_dbg, UART_Handle uart_lora) {

    uint8_t nf;
    uint8_t wait;
    uint8_t ret;
    uint8_t sz;
    unsigned char Mess[32];
    unsigned char buf[256];


    nf=0;
    do {
        if (nf<3) {
            wait = 30;
        } else if (nf<6){
            wait = 60;
        } else {
            break;
        }
        ret = Join_Otaa_Lora(uart_lora);
        // GPIO_write(Board_TIME_MEAS,0);
        if (ret==SUCCESS_OTAA_LORA) {
            strcpy(Mess,"Join_Otaa_Lora() Success\r\n");
            UART_write(uart_dbg, Mess, 26);
        } else {
            strcpy(Mess,"Join_Otaa_Lora() Failed ");
            UART_write(uart_dbg, Mess, 24);
            Mess[0] = '('; Mess[1] = ret+48; Mess[2]=')';
            UART_write(uart_dbg, Mess,3);
            UART_write(uart_dbg, "\r\n",2);
            if (ret==ERROR_OTAA_DENIED) {
                break;
            }
            nf++;
            sleep(wait);
        }
    } while (ret!=SUCCESS_OTAA_LORA);

    return ret;
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
uint8_t Tx_Uncnf_Lora(UART_Handle uart, struct LoraNode *MyLoraNode, uint8_t *mask, uint16_t *nodeId) {

    unsigned char Command[256];
    unsigned char buf[40];
    unsigned char payload[40];
    uint8_t bytes[20];

    uint8_t PortNo;
    uint8_t sz;
    uint8_t ret;
    uint8_t LastAnswer = FALSE;

    memset(bytes,0, sizeof(bytes));

    sprintf(Command,"mac tx uncnf %d ", MyLoraNode->PortNoTx);
    strncat(Command,MyLoraNode->DataTx, MyLoraNode->DataLenTx);
    strcat(Command,"\r\n");
    UART_PRINT("%s\n",Command);

    UART_write(uart, Command, strlen(Command));

    *mask = 0;
    while (LastAnswer==FALSE) {
        sz = GetLine_UART(uart, buf);
        if (strncmp(buf,"ok",2)==0) {
            ;
        } else if (strncmp(buf,"mac_tx_ok",9)==0) {
            UART_PRINT("%s\n",buf);
            LastAnswer=TRUE;
            ret = SUCCESS_TX_MAC_TX;
        } else if (strncmp(buf,"mac_rx ",7)==0) {
            UART_PRINT("ENTRO EN MAC_RX");
            sscanf(buf,"mac_rx %d %s\r\n",&PortNo,payload);
            UART_PRINT("%s\n",buf);
            sz = hex2int(payload, strlen(payload), bytes);
            GetLoraServerParams(bytes, sz, MyLoraNode);
            *mask = bytes[0];
            *nodeId = (bytes[1]<<8) | bytes[2];
            LastAnswer=TRUE;
            ret = SUCCESS_TX_MAC_RX;
        } else if (strncmp(buf,"mac_err",7)==0) {
            LastAnswer=TRUE;
            ret = ERROR_TX_MAC_ERR;
        } else if (strncmp(buf,"invalid_data_len",16)==0) {
            UART_PRINT("INVALID_DATA_LEN");
            LastAnswer=TRUE;
            ret = ERROR_TX_INVALID_DATA_LEN;
        } else if (strncmp(buf,"invalid_param",13)==0) {
            LastAnswer=TRUE;
            ret = ERROR_TX_INVALID_PARAM;
        } else if (strncmp(buf,"not_joined",10)==0) {
            LastAnswer=TRUE;
            ret = ERROR_TX_NOT_JOINED;
        } else if (strncmp(buf,"no_free_ch",10)==0) {
            LastAnswer=TRUE;
            ret = ERROR_TX_NO_FREE_CH;
        } else if (strncmp(buf,"silent",6)==0) {
            LastAnswer=TRUE;
            ret = ERROR_TX_SILENT;
        } else if (strncmp(buf,"frame_counter_err_rejoin_needed",31)==0) {
            LastAnswer=TRUE;
            ret = ERROR_TX_REJOIN_NEEDED;
        } else if (strncmp(buf,"busy",4)==0) {
            LastAnswer=TRUE;
            ret = ERROR_TX_BUSY;
        } else if (strncmp(buf,"mac_paused",10)==0) {
            LastAnswer=TRUE;
            ret = ERROR_TX_MAC_PAUSED;
        } else if (strncmp(buf,"invalid_data_len",16)==0) {
            LastAnswer=TRUE;
            ret = ERROR_TX_INVALID_DATA_LEN;
        } else {
            LastAnswer=TRUE;
            ret = ERROR_TX_UNKNOWN;
        }
    }
    return ret;
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
uint8_t Tx_Cnf_Lora(UART_Handle uart, struct LoraNode *MyLoraNode, uint8_t *mask, uint16_t *nodeId) {

    unsigned char Command[256];
    unsigned char buf[32];
    unsigned char payload[40];
    uint8_t bytes[20];
    uint8_t PortNo;
    uint8_t sz;
    uint8_t ret;
    uint8_t LastAnswer = FALSE;

    memset(bytes,0,sizeof(bytes));

    // Set adaptive datarate ON
    Mac_Adr_On(uart);

    // Set Automatic Retransmit ON
    Mac_Ar_On(uart);

    sprintf(Command,"mac tx cnf %d ", MyLoraNode->PortNoTx);
    strncat(Command,MyLoraNode->DataTx, MyLoraNode->DataLenTx);
    strcat(Command,"\r\n");

    UART_write(uart, Command, strlen(Command));

    *mask = 0;
    *nodeId = 0;
    while (LastAnswer==FALSE) {
        sz = GetLine_UART(uart, buf);
        if (strncmp(buf,"ok",2)==0) {
            ;
        }else if (strncmp(buf,"mac_tx_ok",9)==0) {
            LastAnswer=TRUE;
            UART_PRINT("2\r\n");
            ret = SUCCESS_TX_MAC_TX;
        }else if (strncmp(buf,"mac_rx ",7)==0) {
            sscanf(buf,"mac_rx %d %s\r\n",&PortNo,payload);
            UART_PRINT("3\r\n");
            sz = hex2int(payload, strlen(payload), bytes);
            GetLoraServerParams(bytes, sz, MyLoraNode);
            *mask = bytes[0];
            *nodeId = (bytes[1]<<8) | bytes[2];
            ret = SUCCESS_TX_MAC_RX;
        }else if (strncmp(buf,"mac_err",7)==0) {
            UART_PRINT("4\r\n");
            LastAnswer=TRUE;
            ret = ERROR_TX_MAC_ERR;
        }else if (strncmp(buf,"invalid_data_len",16)==0) {
            UART_PRINT("5\r\n");
            LastAnswer=TRUE;
            ret = ERROR_TX_INVALID_DATA_LEN;
        }else if (strncmp(buf,"invalid_param",13)==0) {
            UART_PRINT("6\r\n");
            LastAnswer=TRUE;
            ret = ERROR_TX_INVALID_PARAM;
        }else if (strncmp(buf,"not_joined",10)==0) {
            UART_PRINT("7\r\n");
            LastAnswer=TRUE;
            ret = ERROR_TX_NOT_JOINED;
        }else if (strncmp(buf,"no_free_ch",10)==0) {
            UART_PRINT("8\r\n");
            LastAnswer=TRUE;
            ret = ERROR_TX_NO_FREE_CH;
        }else if (strncmp(buf,"silent",6)==0) {
            UART_PRINT("9\r\n");
            LastAnswer=TRUE;
            ret = ERROR_TX_SILENT;
        }else if (strncmp(buf,"frame_counter_err_rejoin_needed",31)==0) {
            UART_PRINT("10\r\n");
            LastAnswer=TRUE;
            ret = ERROR_TX_REJOIN_NEEDED;
        }else if (strncmp(buf,"busy",4)==0) {
            UART_PRINT("11\r\n");
            LastAnswer=TRUE;
            ret = ERROR_TX_BUSY;
        }else if (strncmp(buf,"mac_paused",10)==0) {
            UART_PRINT("12\r\n");
            LastAnswer=TRUE;
            ret = ERROR_TX_MAC_PAUSED;
        }else if (strncmp(buf,"invalid_data_len",16)==0) {
            UART_PRINT("13\r\n");
            LastAnswer=TRUE;
            ret = ERROR_TX_INVALID_DATA_LEN;
        } else {
            UART_PRINT("14\r\n");
            LastAnswer=TRUE;
            ret = ERROR_TX_UNKNOWN;
        }
    }
    UART_PRINT(ret);
    return ret;
}

/*!
 *  @brief  Function to put the RN2483 module in sleep mode during sleep ms. This function returns when the device comes out of sleep mode
 *
 *
 *  @pre    Sys_Sleep() has been called
 *
 *  @param  uart          Logical peripheral number for the UART port connected to the RN2483 using command "mac tx cnf <PortNo> <DataTx>"
 *
 *  @param  sleep         Duration of the sleep mode in milliseconds. sleep variable must be between 100-(2^32-1) ms
 *
 *  @return               0 if ok, 1 if some error is produced
 *
 */
uint8_t Sys_Sleep(UART_Handle uart, uint32_t sleep) {

    unsigned char Command[256];
    unsigned char buf[32];
    uint8_t sz;

    sprintf(Command,"sys sleep %d\r\n",sleep);

    UART_write(uart, Command, strlen(Command));
    sz = GetLine_UART(uart, buf);

    if (strncmp(buf,"ok",2)==0) {
        return 0;
    } else if (strncmp(buf,"invalid_param",13)==0) {
        return 1;
    } else {
        return 1;
    }
}

uint8_t Sys_FactoryReset(UART_Handle uart) {

    unsigned char Command[256];
    unsigned char buf[32];
    uint8_t sz;

    strcpy(Command,"sys factoryRESET\r\n");

    UART_write(uart, Command, strlen(Command));
    sz = GetLine_UART(uart, buf);

    return 0;
}


// buf is the string that comes after mac_rx response from the RN2483
void GetLoraRxData(unsigned char *buf, uint8_t size, struct LoraNode *MyLoraNode) {

    uint8_t sz;
    uint8_t i, ini;
    const char space =  ' ';
    const char cr = '\r';
    char *bufcp;

    i=0;
    while (buf[i]!=space) {
        bufcp[i]=buf[i];
        i++;
    }

    MyLoraNode->PortNoRx = atoi(bufcp);

    i=i+1;
    ini = i;
    while (buf[i]!=cr) {
        *(MyLoraNode->DataRx+i-ini) = buf[i++];
    }

    MyLoraNode->DataLenRx = (i-ini);

}



uint8_t Uint8Array2Char(uint8_t *DataPacket, uint8_t DataPacketLen, unsigned char *HexStr) {

    uint8_t i, j, k;
    uint16_t a;

    for (i=0;i<DataPacketLen;i++) {
        for (j=0;j<2;j++) {
            a = (DataPacket[i] >> (4-(j<<2))) & 0x0f;
            k = (i<<1)+j;
            *(HexStr+k) = (a<10)? a+48 : a+55;
        }
    }
    HexStr[k+1] = '\0';

    return k+1;

}

uint8_t hex2int(unsigned char *hex, uint8_t hlen, uint8_t *bytes) {

    uint32_t val = 0;
    uint8_t mybyte;
    int i;
    int k=0;

    UART_PRINT("ENTRO hex2int\n\r");
    for (i=0;i<hlen;i++) {
        // get current character then increment
        uint8_t mybyte = hex[i];
        // transform hex character to the 4bit equivalent number, using the ascii table indexes
        if (mybyte >= '0' && mybyte <= '9') mybyte = mybyte - '0';
        else if (mybyte >= 'a' && mybyte <='f') mybyte = mybyte - 'a' + 10;
        else if (mybyte >= 'A' && mybyte <='F') mybyte = mybyte - 'A' + 10;
        // shift 4 to make space for new digit, and add the 4 bits of the new digit

        val = (val << 4) | (mybyte & 0xF);
        if ((i&0x01)==1) {
            bytes[k++]=val;
            val = 0;
        }
    }

    UART_PRINT("K= %d\n\r",k);
    return k;   // Returns the size of bytes array
}

uint8_t GetLoraServerParams(uint8_t *bytes, uint8_t blen, struct LoraNode *MyLoraNode) {

    uint8_t mask = bytes[0];
    int inext = 1;

    UART_PRINT("mask1= %d\n\r", mask);
    UART_PRINT("mask2= %d\n\r", mask);
    UART_PRINT("mask= %d result= %d\n\r", mask, mask&0x10);
    UART_PRINT("ENTRO EN GetLoraServerParams\n\r");
    if ((mask&0x01)!=0) {   // WakeUpInterval
        UART_PRINT("ENTRO EN WakeUpInterval\n\r");
        MyNode.WakeUpInterval = (bytes[inext]<<8) | bytes[inext+1];
        inext = inext+2;
        writeWakeUp(MyNode.WakeUpInterval);
        st_readFileWakeUp();
    }

    if ((mask&0x02)!=0) {
        UART_PRINT("ENTRO EN mode\n\r");
        MyNode.Mode = (bytes[inext] & 0x07);
        inext = inext+1;
        writeMode(MyNode.Mode);
        st_readFileMode();
    }

    if ((mask&0x04)!=0) {
        UART_PRINT("ENTRO EN ssid\n\r");
        MyNode.SSID[0] = (char)(bytes[inext]);
        MyNode.SSID[1] = (char)(bytes[inext+1]);
        MyNode.SSID[2] = (char)(bytes[inext+2]);
        MyNode.SSID[3] = (char)(bytes[inext+3]);
        MyNode.SSID[4] = (char)(bytes[inext+4]);
        MyNode.SSID[5] = (char)(bytes[inext+5]);
        inext = inext + 6;
        writeSSID(MyNode.SSID);
        st_readFileSSID(&(MyNode.SSID));
    }

    if ((mask&0x08)!=0) {
        UART_PRINT("ENTRO EN ncycles\n\r");
        MyNode.NCycles = bytes[inext];
        inext = inext + 1;
        writeNCycles(MyNode.NCycles);
        st_readFileNCycles();
    }

    if ((mask&0x10)!=0) {
        UART_PRINT("ENTRO EN if upctr\n\r");
        MyLoraNode->Upctr = (bytes[inext]<<16) | (bytes[inext+1]<<8) | (bytes[inext+2]);
        inext = inext + 3;
        writeUpCntr(MyLoraNode->Upctr);
        st_readFileUpCntr();
    }

    if ((mask&0x20)!=0) { // ADXL355
        UART_PRINT("ENTRO EN ADXL355\n\r");
        MyADXL.SampleRate = (bytes[inext]<<8) | bytes[inext+1];
        MyADXL.NSamples = (bytes[inext+2]<<8) | bytes[inext+3];
        inext = inext + 4;
    }

    if ((mask&0x40)!=0) { // LDC1000
        UART_PRINT("ENTRO EN LDC1000\n\r");
        MyLDC.NSamples = (bytes[inext]<<8) | bytes[inext+1];
        inext = inext + 2;
    }

    return 0;
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

uint8_t IniPacket(uint8_t *DataPacket, uint16_t NodeId) {

    (*DataPacket) = NodeId;

    return 1;

}

uint8_t AddDataPacket(uint8_t *DataPacket, uint8_t DataPacketLen, uint16_t SensorId, int16_t *DataSensor, uint8_t DataSensorLen) {

    uint8_t i, j, n, a;

    n=DataPacketLen;

     Sensor Info
    DataPacket[n++] = (SensorId & 0xFF00) >> 8;
    DataPacket[n++] = (SensorId & 0x00FF);
    DataPacket[n++] = DataSensorLen;
    for (i=0; i<DataSensorLen;i++) {
        for (j=0;j<2;j++) {
            a = (DataSensor[(i<<1)+j] >> (8-(j<<3))) & 0x00FF;
            DataPacket[n++] = a;
        }
    }

     Return new length of Datapacket
    return n;

}*/

