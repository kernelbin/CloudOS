/*
 * FATDef.h  Definitions & structs for FAT
 * Copyright (c) He yang 2021 (1160386205@qq.com)
 */


#ifndef _FATDEF_H_
#define _FATDEF_H_

#include "DataType.h"

typedef struct _FAT_BPB
{
    BYTE    BootJmp[3];
    CHAR    OEMName[8];
    UINT16  BytesPerSector;
    BYTE    SectorsPerCluster;
    UINT16  ReservedSectorCount;
    BYTE    TableCount;
    UINT16  RootEntryCount;
    UINT16  TotalSectors16;
    UINT8   MediaType;
    UINT16  TableSize;
    UINT16  SectorsPerTrack;
    UINT16  HeadSideCount;
    UINT32  HiddenSectorCount;
    UINT32  TotalSectors32;
}__attribute__((packed)) FAT_BPB, * PFAT_BPB;

typedef struct _FAT_DIRENTRY
{
    CHAR    FileName[11];   /* Filename + extension */
    BYTE    Attr;           /* File attributes */
    BYTE    ReservedNT;     /* Reserved for use by Windows NT */
    BYTE    TimeInTenths;   /* Millisecond stamp at file creation */
    WORD    CreateTime;     /* Time file was created */
    WORD    CreateDate;     /* Date file was created */
    WORD    LastAccessDate; /* Date file was last accessed */
    WORD    ClusterHigh;    /* High word of this entry's start cluster */
    WORD    Time;           /* Time last modified */
    WORD    Date;           /* Date last modified */
    WORD    ClusterLow;     /* First cluster number low word */
    UINT32  Size;           /* File size */
}__attribute__((packed)) FAT_DIRENTRY, * PFAT_DIRENTRY;


VOID InitFAT12Helper();

BOOL FAT12ReadRootDirFile(LPCSTR FileName, PBYTE Buffer, UINT BufferSize);

#endif // _FATDEF_H_
