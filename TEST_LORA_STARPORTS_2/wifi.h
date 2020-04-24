/*
 * wifi.h
 *
 *  Created on: 12 jun. 2019
 *      Author: aperez
 */
#ifndef WIFI_H_
#define WIFI_H_

/////////////////////////////////////

#define PORT                        (9000)
#define DEST_IP_ADDR                SL_IPV4_VAL(192,168,5,1)
#define NUM_OF_PKT                  (10)
#define FRAME_LENGTH                (1000)

#define PORT_UDP                    (9999)

#define SERIALWIFI_PORT             (9000)
/////////////////////////////////////

#define SUCCESS_CONNECT_WIFI 0
#define ERROR_CONNECT_WIFI   1

#define SSID_NAME           "PORTS2"                 /* AP SSID */
#define SECURITY_TYPE       SL_WLAN_SEC_TYPE_WPA_WPA2       /* Security type (OPEN or WEP or WPA*/
#define SECURITY_KEY        "1234567890"                   /* Password of the secured AP */
#define SECURITY_KEY_FILE_CONNECT        "1234567890"                   /* Password of the secured AP */

#define LOOP_FOREVER() \
    { \
        while(1) {; } \
    }

typedef enum
{
    STATUS_BIT_CONNECTION,              // If this bit is set: the device is connected to the AP or client is connected to device (AP)
    STATUS_BIT_IP_ACQUIRED,             // If this bit is set: the device has acquired an IP
}e_StatusBits;


#define SET_STATUS_BIT(status_variable, bit) status_variable |= (1 << (bit))
#define CLR_STATUS_BIT(status_variable, bit) status_variable &= ~(1 << (bit))
#define GET_STATUS_BIT(status_variable, bit) (0 != \
                                              (status_variable & (1 << (bit))))

#define IS_CONNECTED(status_variable)        GET_STATUS_BIT( \
        status_variable, \
        STATUS_BIT_CONNECTION)

#define IS_IP_ACQUIRED(status_variable)       GET_STATUS_BIT( \
        status_variable, \
        STATUS_BIT_IP_ACQUIRED)

#define ASSERT_ON_ERROR(error_code) \
    { \
        if(error_code < 0) \
        { \
            ERR_PRINT(error_code); \
            return error_code; \
        } \
    }


#define SL_SOCKET_ERROR         ( \
        "Socket error, please refer \"SOCKET ERRORS CODES\" section in errors.h")

/* Control block definition */
typedef struct _PowerMeasure_ControlBlock_t_
{
    uint32_t        slStatus;               //SimpleLink Status
    signed char     frameData[FRAME_LENGTH];
    SlSockAddrIn_t  ipV4Addr;
}PowerMeasure_ControlBlock;


int32_t wlanConnect(void);
int32_t wlanConnectFromFile(unsigned char *ssid);
int32_t wlanConf(void);
void SimpleLinkNetAppRequestMemFreeEventHandler (uint8_t *buffer);
void SimpleLinkNetAppRequestEventHandler(SlNetAppRequest_t *pNetAppRequest, SlNetAppResponse_t *pNetAppResponse);
void SimpleLinkWlanEventHandler(SlWlanEvent_t *pWlanEvent);
void SimpleLinkFatalErrorEventHandler(SlDeviceFatal_t *slFatalErrorEvent);
void SimpleLinkNetAppEventHandler(SlNetAppEvent_t *pNetAppEvent);
void SimpleLinkHttpServerEventHandler( SlNetAppHttpServerEvent_t *pHttpEvent, SlNetAppHttpServerResponse_t *pHttpResponse);
void SimpleLinkGeneralEventHandler(SlDeviceEvent_t *pDevEvent);
void SimpleLinkSockEventHandler(SlSockEvent_t *pSock);
void SimpleLinkSocketTriggerEventHandler(SlSockTriggerEvent_t *pSlTriggerEvent);

void prepareDataFrame(uint16_t Port, uint32_t IpAddr);
int16_t sendTcpClient(uint16_t port);
int32_t startTcpServer(void);
//*****************************************************************************
// Globals
//*****************************************************************************
PowerMeasure_ControlBlock   PowerMeasure_CB;


#endif /* WIFI_H_ */
