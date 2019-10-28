/*
 * file_system.h
 *
 *  Created on: 29 ago. 2019
 *      Author: aperez
 */

#ifndef FILE_SYSTEM_H_
#define FILE_SYSTEM_H_

#include "hal_LORA.h"
#include <ti/drivers/net/wifi/simplelink.h>

/////////file system//////////////
#define MAX_FILE_SIZE 256
#define FS_WAKEUP       "wake_up_time"
#define FS_MODE         "mode"
#define FS_NCYCLES      "ncycles"
#define FS_SSID         "ssid"
#define FS_FIRST_BOOT   "firstboot"
#define FS_NFAILS       "nfails"
#define FS_UPCNTR       "upcntr"
////////////////////////

#define CERTIFICATE "dummy-trusted-cert"
#define MAX_FILE_ENTRIES 4


typedef struct
{
    SlFileAttributes_t attribute;
    char fileName[SL_FS_MAX_FILE_NAME_LENGTH];
} slGetfileList_t;



int st_showStorageInfo();
int st_listFiles(int bShowDescription);
int writeNFails(uint16_t NFails);
int st_readFileNFails();
int writeFirstBoot(uint8_t FirstBoot);
int st_readFileFirstBoot();
int st_readFileFirstBoot();
int writeNCycles(uint8_t NCycles);
int st_readFileNCycles();
int writeUpCntr(uint32_t Upctr);
int st_readFileUpCntr();
int writeMode(uint8_t Mode);
int st_readFileMode();
int writeWakeUp(uint16_t WakeUpInterval);
int st_readFileWakeUp();
int writeSSID(unsigned char *SSID);
int st_readFileSSID(unsigned char *ssid);
int deleteFile();
int newFile();

#endif /* FILE_SYSTEM_H_ */
