/*
 * wifi.c
 *
 *  Created on: 12 jun. 2019
 *      Author: aperez
 */
//*****************************************************************************
// Includes
//*****************************************************************************
#include <ti/drivers/UART.h>
#include <ti/drivers/net/wifi/simplelink.h>
#include "wifi.h"
#include "hal_UART.h"

//*****************************************************************************
// Variables
//*****************************************************************************
//uint32_t        slStatus;       // set & get status connection

//*****************************************************************************
// SimpleLink Callback Functions
//*****************************************************************************
void SimpleLinkNetAppRequestMemFreeEventHandler (uint8_t *buffer){}

void SimpleLinkNetAppRequestEventHandler(SlNetAppRequest_t *pNetAppRequest, SlNetAppResponse_t *pNetAppResponse){}

void SimpleLinkHttpServerEventHandler( SlNetAppHttpServerEvent_t *pHttpEvent, SlNetAppHttpServerResponse_t *pHttpResponse){}

void SimpleLinkSockEventHandler(SlSockEvent_t *pSock){}

void SimpleLinkSocketTriggerEventHandler(SlSockTriggerEvent_t *pSlTriggerEvent){}

void SimpleLinkGeneralEventHandler(SlDeviceEvent_t *pDevEvent)
{
    UART_PRINT("[GENERAL EVENT] - ID=[%d] Sender=[%d]\n\n",
               pDevEvent->Data.Error.Code,
               pDevEvent->Data.Error.Source);
}

void SimpleLinkWlanEventHandler(SlWlanEvent_t *pWlanEvent)
{
    switch(pWlanEvent->Id)
    {
        case SL_WLAN_EVENT_CONNECT:
        {

            SET_STATUS_BIT(PowerMeasure_CB.slStatus, STATUS_BIT_CONNECTION);
        }
        break;

        case SL_WLAN_EVENT_DISCONNECT:
        {
            CLR_STATUS_BIT(PowerMeasure_CB.slStatus, STATUS_BIT_CONNECTION);
            CLR_STATUS_BIT(PowerMeasure_CB.slStatus, STATUS_BIT_IP_ACQUIRED);
        }
        break;

        default:
        {
            UART_PRINT("[WLAN EVENT] Unexpected event [0x%x]\n\r",
                       pWlanEvent->Id);
        }
        break;
    }
}

void SimpleLinkFatalErrorEventHandler(SlDeviceFatal_t *slFatalErrorEvent)
{
    switch (slFatalErrorEvent->Id)
    {
        case SL_DEVICE_EVENT_FATAL_DEVICE_ABORT:
        {
        UART_PRINT(
            "[ERROR] - FATAL ERROR: Abort NWP event "
            "detected: AbortType=%d, AbortData=0x%x\n\r",
            slFatalErrorEvent->Data.DeviceAssert.Code,
            slFatalErrorEvent->Data.DeviceAssert.Value);
        }
        break;

        case SL_DEVICE_EVENT_FATAL_DRIVER_ABORT:
        {
            UART_PRINT("[ERROR] - FATAL ERROR: Driver Abort detected. \n\r");
        }
        break;

        case SL_DEVICE_EVENT_FATAL_NO_CMD_ACK:
        {
        UART_PRINT(
            "[ERROR] - FATAL ERROR: "
            "No Cmd Ack detected [cmd opcode = 0x%x] \n\r",
            slFatalErrorEvent->Data.NoCmdAck.Code);
        }
        break;

        case SL_DEVICE_EVENT_FATAL_SYNC_LOSS:
        {
            UART_PRINT("[ERROR] - FATAL ERROR: Sync loss detected n\r");
        }
        break;

        case SL_DEVICE_EVENT_FATAL_CMD_TIMEOUT:
        {
        UART_PRINT(
            "[ERROR] - FATAL ERROR: Async event timeout "
            "detected [event opcode =0x%x]  \n\r",
            slFatalErrorEvent->Data.CmdTimeout.Code);
        }
        break;

        default:
            UART_PRINT("[ERROR] - FATAL ERROR:"
                       " Unspecified error detected \n\r");
        break;
    }
}

void SimpleLinkNetAppEventHandler(SlNetAppEvent_t *pNetAppEvent)
{
    switch(pNetAppEvent->Id)
    {
        case SL_NETAPP_EVENT_IPV4_ACQUIRED:
        {
            SET_STATUS_BIT(PowerMeasure_CB.slStatus, STATUS_BIT_IP_ACQUIRED);
            UART_PRINT("[NETAPP EVENT] IP Acquired: IP=%d.%d.%d.%d , "
            "Gateway=%d.%d.%d.%d\n\r",
            SL_IPV4_BYTE(pNetAppEvent->Data.IpAcquiredV4.Ip,3),
            SL_IPV4_BYTE(pNetAppEvent->Data.IpAcquiredV4.Ip,2),
            SL_IPV4_BYTE(pNetAppEvent->Data.IpAcquiredV4.Ip,1),
            SL_IPV4_BYTE(pNetAppEvent->Data.IpAcquiredV4.Ip,0),
            SL_IPV4_BYTE(pNetAppEvent->Data.IpAcquiredV4.Gateway,3),
            SL_IPV4_BYTE(pNetAppEvent->Data.IpAcquiredV4.Gateway,2),
            SL_IPV4_BYTE(pNetAppEvent->Data.IpAcquiredV4.Gateway,1),
            SL_IPV4_BYTE(pNetAppEvent->Data.IpAcquiredV4.Gateway,0));
        }
        break;

        default:
        {
            UART_PRINT("[NETAPP EVENT] Unexpected event [0x%x] \n\r",
                       pNetAppEvent->Id);
        }
        break;
    }
}

//*****************************************************************************
// Local Functions
//*****************************************************************************

int32_t wlanConf(void){

    /* START DEVICE */
    sl_Start(0, 0, 0);

    /* SET ROLE AS STATION*/
    sl_WlanSetMode(ROLE_STA);

    /* Set PowerManagement policy to normal */
    sl_WlanPolicySet(SL_WLAN_POLICY_PM , SL_WLAN_NORMAL_POLICY, NULL, 0);

    /* Connection policy Auto  */
    sl_WlanPolicySet(SL_WLAN_POLICY_CONNECTION, SL_WLAN_CONNECTION_POLICY(1, 0, 0, 0), NULL, 0);

    /* set IP DHCP */
    sl_NetCfgSet(SL_NETCFG_IPV4_STA_ADDR_MODE, SL_NETCFG_ADDR_DHCP, 0, 0);

    return(0);
}

//*****************************************************************************
//
//! \brief    This function connects to a WIFI AP.
//!
//!
//!
//!
//
//*****************************************************************************
int32_t wlanConnect(void)
{
    SlWlanSecParams_t secParams = {0};

    secParams.Key = (signed char*)SECURITY_KEY;
    secParams.KeyLen = strlen(SECURITY_KEY);
    secParams.Type = SECURITY_TYPE;

    /* WLAN CONNECT TO AP*/
    sl_WlanConnect((signed char*)SSID_NAME, strlen(SSID_NAME), 0, &secParams, 0);
    UART_PRINT("Trying to connect to AP : %s\n\r", SSID_NAME);

    sl_Task(NULL);

    while((!IS_CONNECTED(PowerMeasure_CB.slStatus)) || (!IS_IP_ACQUIRED(PowerMeasure_CB.slStatus)))
    {
        sl_Task(NULL);
    }
    return(0);
}

//*****************************************************************************
//
//! \brief    This function connects to a WIFI AP.
//!
//!
//!
//!
//
//*****************************************************************************
int32_t wlanConnectFromFile(unsigned char *ssid)
{
    uint8_t ret;

    SlWlanSecParams_t secParams = {0};

    secParams.Key = (signed char*)SECURITY_KEY_FILE_CONNECT;
    secParams.KeyLen = strlen(SECURITY_KEY_FILE_CONNECT);
    secParams.Type = SECURITY_TYPE;

    unsigned char FileSSID[13];

    sprintf(&FileSSID,"%s",ssid );

    /* WLAN CONNECT TO AP*/
    ret = sl_WlanConnect((signed char*)FileSSID, strlen(FileSSID), 0, &secParams, 0);
    UART_PRINT("Trying to connect to AP : %s\n\r", FileSSID);

    sl_Task(NULL);

    while((!IS_CONNECTED(PowerMeasure_CB.slStatus)) || (!IS_IP_ACQUIRED(PowerMeasure_CB.slStatus)))
    {
        sl_Task(NULL);
    }

    if (ret!=0){
        return ERROR_CONNECT_WIFI;
    }else{
        return SUCCESS_CONNECT_WIFI;
    }
    return(0);
}

void prepareDataFrame(uint16_t port,uint32_t ipAddr)
{
    uint16_t idx;

    for(idx = 0;idx < FRAME_LENGTH;idx++)
    {
        PowerMeasure_CB.frameData[idx] = (signed char)(idx % 255);
    }
    PowerMeasure_CB.ipV4Addr.sin_family = SL_AF_INET;
    PowerMeasure_CB.ipV4Addr.sin_port = sl_Htons(port);
    PowerMeasure_CB.ipV4Addr.sin_addr.s_addr = sl_Htonl(ipAddr);
}
/*
int16_t sendTcpClient(uint16_t port)
{
    int16_t         sockId;
    int16_t         idx = 0;
    int16_t         status = -1;
    char*    buffer = "sdfdfsf";

    sockId = sl_Socket(SL_AF_INET,SL_SOCK_STREAM, 0);
    ASSERT_ON_ERROR(sockId);

    UART_PRINT("TRYING TO CONNECT CLIENT %s\n\r");
    status = sl_Connect(sockId, ( SlSockAddr_t *)&PowerMeasure_CB.ipV4Addr, sizeof(SlSockAddrIn_t));
    if(status < 0)
    {
        sl_Close(sockId);
        ASSERT_ON_ERROR(sockId);
        UART_PRINT("CLIENT REFUSED : %s\n\r", ( SlSockAddr_t *)&PowerMeasure_CB.ipV4Addr);
    }
    else{
        UART_PRINT("CLIENT CONNECTED : %s\n\r",( SlSockAddr_t *)&PowerMeasure_CB.ipV4Addr);
    }

    while (idx < NUM_OF_PKT)
    {
        //status = sl_Send(sockId,PowerMeasure_CB.frameData ,FRAME_LENGTH, 0 );
        status = sl_Send(sockId, buffer, sizeof(buffer), 0 );
        if(status <= 0 )
        {
            status = sl_Close(sockId);
            ASSERT_ON_ERROR(status);
            UART_PRINT("ERROR SENDING PACKET : %s\n\r", buffer);
        }
        else{
            UART_PRINT("PACKET SENT SUCCESSFULLY : %s\n\r", buffer);
        }
        idx++;
    }

    status = sl_Close(sockId);

    return(0);
}
*/
int16_t sendUdpClient(uint16_t port)
{
    int16_t         sockId;
    int16_t         idx = 0;
    int32_t         status = -1;

    sockId = sl_Socket(SL_AF_INET,SL_SOCK_DGRAM, 0);

    if(sockId > 0){
        UART_PRINT("SOCKET UDP");
    }else{
        UART_PRINT("error UDP");
    }

    while (idx < NUM_OF_PKT)
    {
        status = sl_SendTo(sockId,PowerMeasure_CB.frameData ,FRAME_LENGTH , 0,(SlSockAddr_t *)&PowerMeasure_CB.ipV4Addr,sizeof(SlSockAddrIn_t));
        if(status <= 0 )
        {
            status = sl_Close(sockId);
            ASSERT_ON_ERROR(sockId);
        }
        idx++;
    }

    return(0);
}
int32_t startTcpServer(void)
{
    SlSockAddr_t localAddr;
    uint32_t nonBlocking;
    int32_t retVal;
    int32_t sock;

    localAddr.sa_family = SL_AF_INET;
    localAddr.sa_data[0] = ((SERIALWIFI_PORT >> 8) & 0xFF);
    localAddr.sa_data[1] = (SERIALWIFI_PORT & 0xFF);

    //all 0 => Own IP address
    memset(&localAddr.sa_data[2], 0, 4);

    sock = sl_Socket(SL_AF_INET, SL_SOCK_STREAM, 0);
    if(sock < 0)
    {
        UART_PRINT("FAILED CREATING SERVER\r\n");
        return(-1);
    }
    else{
        UART_PRINT("SERVER CREATED! \r\n");
        usleep(1000000);
    }

    nonBlocking = 1;
    retVal =
        sl_SetSockOpt(sock, SL_SOL_SOCKET,
                      SL_SO_NONBLOCKING,
                      &nonBlocking,
                      sizeof(nonBlocking));
    if(retVal < 0)
    {
        UART_PRINT("Failed to set socket options 21\r\n");
        sl_Close(sock);
        return(-1);
    }

    if(sl_Bind(sock, &localAddr, sizeof(localAddr)) < 0)
    {
        UART_PRINT("BIND ERROR\n\r", ERR_PRINT(sock));
        sl_Close(sock);
        return(-1);
    }

    UART_PRINT("SERVER BINDED! \r\n");
    usleep(100000);

    if(sl_Listen(sock, 0) < 0)
    {
        UART_PRINT(" Listen Error\n\r");
        sl_Close(sock);
        return(-1);
    }

    UART_PRINT("SERVER LISTENING! %s\n\r", &localAddr.sa_data[2]);
    usleep(100000);
//    sl_Close(sock);

    return(0);
}
