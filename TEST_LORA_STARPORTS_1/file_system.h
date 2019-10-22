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
int writeFileKeys(_i32 fileHandle, struct LoraNode *MyLoraNode);
int writeFileUpCntr(struct LoraNode* MyLoraNode);
int readFile( _i32 fileHandle);
int openRead(const _u8 *filename);
int deleteFile();
int newFile();

#endif /* FILE_SYSTEM_H_ */
