/*
 * file_system.h
 *
 *  Created on: 29 ago. 2019
 *      Author: aperez
 */

#ifndef FILE_SYSTEM_H_
#define FILE_SYSTEM_H_

////////////////////////
#define MAX_FILE_SIZE 100
#define FILENAME "STARPORTS_FS"
#define CERTIFICATE "dummy-trusted-cert"
#define MAX_FILE_ENTRIES 4


typedef struct
{
    SlFileAttributes_t attribute;
    char fileName[SL_FS_MAX_FILE_NAME_LENGTH];
} slGetfileList_t;



int st_showStorageInfo();
int st_listFiles(int bShowDescription);
static int writeFile(_i32 fileHandle, _u32 length, _const char *buffer);
static int readFile( _i32 fileHandle, _u32 length, _i8 *buffer);
int openRead(const _u8 *filename);
int deleteFile();
int newFile();

#endif /* FILE_SYSTEM_H_ */
