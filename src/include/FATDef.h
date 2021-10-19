/*
 * FATDef.h  Definitions & structs for FAT
 * Copyright (c) He yang 2021 (1160386205@qq.com)
 */


#ifndef _FATDEF_H_
#define _FATDEF_H_

#include <stdint.h>

typedef struct _FAT_BPB
{
    uint8_t     BootJmp[3];
    char        OEMName[8];
    uint16_t    BytesPerSector;
    uint8_t     SectorsPerCluster;
    uint16_t    ReservedSectorCount;
    uint8_t     TableCount;
    uint16_t    RootEntryCount;
    uint16_t    TotalSectors16;
    uint8_t     MediaType;
    uint16_t    TableSize;
    uint16_t    SectorsPerTrack;
    uint16_t    HeadSideCount;
    uint32_t    HiddenSectorCount;
    uint32_t    TotalSectors32;
 
}__attribute__((packed)) FAT_BPB, * PFAT_BPB;

typedef struct _FAT_DIRENTRY
{
    char        FileName[11];   /* Filename + extension */
    uint8_t     Attr;           /* File attributes */
    uint8_t     ReservedNT;     /* Reserved for use by Windows NT */
    uint8_t     TimeInTenths;   /* Millisecond stamp at file creation */
    uint16_t    CreateTime;     /* Time file was created */
    uint16_t    CreateDate;     /* Date file was created */
    uint16_t    LastAccessDate; /* Date file was last accessed */
    uint16_t    ClusterHigh;    /* High word of this entry's start cluster */
    uint16_t    Time;           /* Time last modified */
    uint16_t    Date;           /* Date last modified */
    uint16_t    ClusterLow;     /* First cluster number low word */
    uint32_t    Size;           /* File size */
}__attribute__((packed)) FAT_DIRENTRY, * PFAT_DIRENTRY;


void InitFAT12Helper();

int FAT12ReadRootDirFile(char FileName[], unsigned char *Buffer, unsigned int BufferSize);

#endif // _FATDEF_H_
