/*
 * file_system.c
 *
 *  Created on: 29 ago. 2019
 *      Author: aperez
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdarg.h>

/* Driver Header files */
#include <ti/drivers/UART.h>
#include <ti/drivers/net/wifi/simplelink.h>

#include "platform.h"

#include <ti/devices/cc32xx/inc/hw_types.h>
#include <ti/devices/cc32xx/driverlib/prcm.h>

#include "hal_UART.h"
#include "file_system.h"
#include "STARPORTS_App.h"

/* Example/Board Header files */
#include "Board.h"


extern UART_Handle uart0;
////////////////////////////////////
/*Muestra info sobre memoria etc..*/
////////////////////////////////////

int st_showStorageInfo()
{
    _i32        RetVal = 0;
    _i32        size;
    _i32        used;
    _i32        avail;
    SlFsControlGetStorageInfoResponse_t storageInfo;

    UART_PRINT("\n\rGet Storage Info:\n\r");

    RetVal = sl_FsCtl(( SlFsCtl_e)SL_FS_CTL_GET_STORAGE_INFO, 0, NULL, NULL, 0, (_u8 *)&storageInfo, sizeof(SlFsControlGetStorageInfoResponse_t), NULL );

    if(RetVal < 0)
    {
        UART_PRINT("sl_FsCtl error: %d\n\r");
    }

    size = (storageInfo.DeviceUsage.DeviceBlocksCapacity *storageInfo.DeviceUsage.DeviceBlockSize) / 1024;
    UART_PRINT("Total space: %dK\n\r\n\r", size);
    UART_PRINT("Filesystem      Size \tUsed \tAvail\t\n\r");
    size = ((storageInfo.DeviceUsage.NumOfAvailableBlocksForUserFiles + storageInfo.DeviceUsage.NumOfAllocatedBlocks) *storageInfo.DeviceUsage.DeviceBlockSize) / 1024;
    used = (storageInfo.DeviceUsage.NumOfAllocatedBlocks *storageInfo.DeviceUsage.DeviceBlockSize) / 1024;
    avail = (storageInfo.DeviceUsage.NumOfAvailableBlocksForUserFiles *storageInfo.DeviceUsage.DeviceBlockSize) / 1024;
    UART_PRINT("%-15s %dK \t%dK \t%dK \t\n\r", "User", size, used, avail);
    size = (storageInfo.DeviceUsage.NumOfReservedBlocksForSystemfiles *storageInfo.DeviceUsage.DeviceBlockSize) / 1024;
    UART_PRINT("%-15s %dK \n\r", "System", size);
    size = (storageInfo.DeviceUsage.NumOfReservedBlocks *storageInfo.DeviceUsage.DeviceBlockSize) / 1024;
    UART_PRINT("%-15s %dK \n\r", "Reserved", size);
    UART_PRINT("\n\r");
    UART_PRINT("\n\r");
    UART_PRINT("%-32s: %d \n\r", "Max number of files",storageInfo.FilesUsage.MaxFsFiles);
    UART_PRINT("%-32s: %d \n\r", "Max number of system files",storageInfo.FilesUsage.MaxFsFilesReservedForSysFiles);
    UART_PRINT("%-32s: %d \n\r", "Number of user files",storageInfo.FilesUsage.ActualNumOfUserFiles);
    UART_PRINT("%-32s: %d \n\r", "Number of system files",storageInfo.FilesUsage.ActualNumOfSysFiles);
    UART_PRINT("%-32s: %d \n\r", "Number of alert",storageInfo.FilesUsage.NumOfAlerts);
    UART_PRINT("%-32s: %d \n\r", "Number Alert threshold",storageInfo.FilesUsage.NumOfAlertsThreshold);
    UART_PRINT("%-32s: %d \n\r", "FAT write counter",storageInfo.FilesUsage.FATWriteCounter);
    UART_PRINT("%-32s: ", "Bundle state");

    if(storageInfo.FilesUsage.Bundlestate == SL_FS_BUNDLE_STATE_STOPPED)
    {
        UART_PRINT("%s \n\r", "Stopped");
    }
    else if(storageInfo.FilesUsage.Bundlestate == SL_FS_BUNDLE_STATE_STARTED)
    {
        UART_PRINT("%s \n\r", "Started");
    }
    else if(storageInfo.FilesUsage.Bundlestate == SL_FS_BUNDLE_STATE_PENDING_COMMIT)
    {
        UART_PRINT("%s \n\r", "Commit pending");
    }

    UART_PRINT("\n\r");

    return RetVal;
}

///////////////////////////////////////////
/*Lista los archivos guardados en memoria*/
///////////////////////////////////////////

int st_listFiles(int bShowDescription)
{
    int retVal = SL_ERROR_BSD_ENOMEM;
    _i32            index = -1;
    _i32 fileCount = 0;
    _i32 numOfFiles = 255;
    slGetfileList_t *buffer = malloc(MAX_FILE_ENTRIES * sizeof(slGetfileList_t));

    UART_PRINT("\n\rRead files list:\n\r");
    if(buffer)
    {
        while( numOfFiles > 0 )
        {
            _i32 i;
            _i32 numOfEntries = (numOfFiles < MAX_FILE_ENTRIES) ? numOfFiles : MAX_FILE_ENTRIES;

            // Get FS list
            retVal = sl_FsGetFileList(&index,
            numOfEntries,
            sizeof(slGetfileList_t),
            (_u8*)buffer,
            SL_FS_GET_FILE_ATTRIBUTES);
            if(retVal < 0)
            {
                UART_PRINT("sl_FsGetFileList error:  %d\n\r", retVal);
                break;
            }
            if(retVal == 0)
            {
                break;
            }

            // Print single column format
            for (i = 0; i < retVal; i++)
            {
                UART_PRINT("[%3d] ", ++fileCount);
                UART_PRINT("%-40s\t", buffer[i].fileName);
                UART_PRINT("%8d\t", buffer[i].attribute.FileMaxSize);
                UART_PRINT("0x%03x\t", buffer[i].attribute.Properties);
                UART_PRINT("\n\r");
            }
            numOfFiles -= retVal;
        }
        UART_PRINT("\n\r");
        if(bShowDescription)
        {
            UART_PRINT(" File properties flags description:\n\r");
            UART_PRINT(" 0x001 - Open file commit\n\r");
            UART_PRINT(" 0x002 - Open bundle commit\n\r");
            UART_PRINT(" 0x004 - Pending file commit\n\r");
            UART_PRINT(" 0x008 - Pending bundle commit\n\r");
            UART_PRINT(" 0x010 - Secure file\n\r");
            UART_PRINT(" 0x020 - No file safe\n\r");
            UART_PRINT(" 0x040 - System file\n\r");
            UART_PRINT(" 0x080 - System with user access\n\r");
            UART_PRINT(" 0x100 - No valid copy\n\r");
            UART_PRINT(" 0x200 - Public write\n\r");
            UART_PRINT(" 0x400 - Public read\n\r");
            UART_PRINT("\n\r");
        }
        free (buffer);
    }

    return retVal;
}

////////////////////////
/* Escribir en archivo*/
///////////////////////
/*
int writeFileKeys(_i32 fileHandle, struct LoraNode* MyLoraNode)
{

    int RetVal = 0;
    _i32 offset = 0;
    _i32 fd;
    unsigned char loraData[256];
    loraData[0]=0x00;

    strcat(&loraData,"\r\nDevAddr: ");
    strncat(&loraData, MyLoraNode->DevAddr,8);

    strcat(&loraData,"\r\nDevEui: ");
    strncat(&loraData, MyLoraNode->DevEui,16);

    strcat(&loraData,"\r\nAppEui: ");
    strncat(&loraData, MyLoraNode->AppEui,16);

    strcat(&loraData,"\r\nAppKey: ");
    strncat(&loraData, MyLoraNode->AppKey,32);

    strcat(&loraData,"\r\nNwksKey: ");
    strncat(&loraData,MyLoraNode->NwksKey,32);

    strcat(&loraData,"\r\nAppsKey: ");
    strncat(&loraData, MyLoraNode->AppsKey,32);


    fd = sl_FsOpen(FILENAMEKEYS, SL_FS_OVERWRITE, 0);
    if (fd < 0)
    {
        UART_PRINT("Error opening the file : %s\n\r", FILENAMEKEYS);
    }
    else
    {
        UART_PRINT("file opened: %s\n\r", FILENAMEKEYS);

        RetVal = sl_FsWrite(fd, 0, loraData, strlen(loraData));
        if (RetVal <= 0)
        {
            UART_PRINT("Writing error:  %d\n\r" ,RetVal);
            return RetVal;
        }

        UART_PRINT("file: %s\n\r", loraData);
        UART_PRINT("Wrote %d bytes...\n\r", RetVal);

        RetVal = sl_FsClose(fd, 0, 0, 0);
        if (RetVal < 0)
        {
            UART_PRINT("Error closing the file : %s\n\r", FILENAMEKEYS);
        }
        else
        {
            UART_PRINT("File closed : %s\n\r", FILENAMEKEYS);
        }
    }

    return offset;
}
*/
///////////////////
//WRITEUPCNTR
///////////////////
int writeFileUpCntr(struct LoraNode* MyLoraNode)
{

    int RetVal = 0;
    _i32 fd;
    _i32 offset = 0;
    unsigned char loraData[256];
    loraData[0]=0x00;

    strncpy(&loraData, MyLoraNode->Upctr,32);

    fd = sl_FsOpen(FS_UPCNTR, SL_FS_OVERWRITE, 0);
    if (fd < 0)
    {
        UART_PRINT("Error opening the file : %s\n\r", FS_UPCNTR);
    }
    else
    {
//        UART_PRINT("file opened for write: %s\n\r", FILENAMEUPCNTR);

        RetVal = sl_FsWrite(fd, 0, loraData, strlen(loraData));
        if (RetVal <= 0)
        {
            UART_PRINT("Writing error:  %d\n\r" ,RetVal);
            return RetVal;
        }

        UART_PRINT("WRITING UPCNTR: %s\n\r", loraData);
//        UART_PRINT("Wrote %d bytes...\n\r", RetVal);

        RetVal = sl_FsClose(fd, 0, 0, 0);
        if (RetVal < 0)
        {
            UART_PRINT("Error closing the file : %s\n\r", FS_UPCNTR);
        }
        else
        {
//            UART_PRINT("File closed for write: %s\n\r", FILENAMEUPCNTR);
        }
    }

    return offset;
}

//////////////////////////////////////
//READ UPCNTR FILE////////////////////
//////////////////////////////////////
int st_readFileUpcntr()
{
    int offset = 0;
    int RetVal = 0;
    _i8 buffer[MAX_FILE_SIZE];
//    _u32 length = strlen(buffer);

    int32_t fd;

    fd = sl_FsOpen(FS_UPCNTR, SL_FS_READ, 0);
    if (fd < 0)
    {
        UART_PRINT("\r\nError opening the file : %s\n\r", FS_UPCNTR);
    }
    else
    {
//        UART_PRINT("\r\nfile opened for read: %s\n\r", FILENAMEUPCNTR);
    }

    //while(offset < length)
    while(RetVal=sl_FsRead(fd, offset, &buffer[0], MAX_FILE_SIZE)>1)
    {
        if(strlen(buffer)!=0)
            {
        if(RetVal == SL_ERROR_FS_OFFSET_OUT_OF_RANGE)
        {// EOF
            break;
        }
        if(RetVal < 0)
        {// Error
            UART_PRINT("sl_FsRead error: %d\n\r", RetVal);
            return RetVal;
        }
        offset += strlen(buffer);
        }

    }
    UART_PRINT("READING UPCNTR: %d \n\r", atoi(&buffer));

    RetVal = sl_FsClose(fd, 0, 0, 0);
    if (RetVal < 0)
    {
        UART_PRINT("Error closing the file : %s\n\r", FS_UPCNTR);
    }
    else
    {
//        UART_PRINT("File closed for read: %s\n\r", FILENAMEUPCNTR);
    }

    return (&buffer);
}
///////////////////
//WRITE Nfails
///////////////////

/*
int writeNfails(struct LoraNode* MyLoraNode)
{

    int RetVal = 0;
    _i32 fd;
    _i32 offset = 0;
    unsigned char loraData[256];
    loraData[0]=0x00;

    strncpy(&loraData, MyLoraNode->Upctr,32);

    fd = sl_FsOpen(FILENAMENFAILS, SL_FS_OVERWRITE, 0);
    if (fd < 0)
    {
        UART_PRINT("Error opening the file : %s\n\r", FILENAMENFAILS);
    }
    else
    {
//        UART_PRINT("file opened for write: %s\n\r", FILENAMENFAILS);

        RetVal = sl_FsWrite(fd, 0, loraData, strlen(loraData));
        if (RetVal <= 0)
        {
            UART_PRINT("Writing error:  %d\n\r" ,RetVal);
            return RetVal;
        }

        UART_PRINT("WRITING UPCNTR: %s\n\r", loraData);
//        UART_PRINT("Wrote %d bytes...\n\r", RetVal);

        RetVal = sl_FsClose(fd, 0, 0, 0);
        if (RetVal < 0)
        {
            UART_PRINT("Error closing the file : %s\n\r", FILENAMENFAILS);
        }
        else
        {
//            UART_PRINT("File closed for write: %s\n\r", FILENAMENFAILS);
        }
    }

    return offset;
}
*/
/////////////////////
/*Eliminar archivo*/
////////////////////
int deleteFile()
{
    UART_PRINT("\n\r\n\r ** Deleting the file\n\r");
    sl_FsDel(FS_UPCNTR, 0);
    st_listFiles(0);
    return 0;
}

////////////////////////////////
/*Crea archivo en File system*/
///////////////////////////////
int newFile()
{
    int RetVal = 0;
    _i32 fd;

    fd = sl_FsOpen(FS_UPCNTR, (SL_FS_CREATE | SL_FS_CREATE_FAILSAFE | SL_FS_CREATE_MAX_SIZE(MAX_FILE_SIZE)), 0);
    if(fd<0)
    {
        UART_PRINT("Creating file error: %d\n\r", fd);
    }
    else
    {
        fd = sl_FsClose(fd, 0, 0, 0);
        if (fd < 0)
        {
            UART_PRINT("Closing file error: %d\n\r", RetVal);
        }
    }

    return fd;
}


