#include "FatDef.h"
#include "AsmFunc.h"


#define ATA_STATUS_BSY  0x80 // Indicates the drive is preparing to send/receive data (wait for it to clear). In case of 'hang' (it never clears), do a software reset.
#define ATA_STATUS_RDY  0x40 // Bit is clear when drive is spun down, or after an error. Set otherwise.
#define ATA_STATUS_DRQ  0x08 // Set when the drive has PIO data to transfer, or is ready to accept PIO data.
#define ATA_STATUS_DF   0x20 // Drive Fault Error (does not set ERR).
#define ATA_STATUS_ERR  0x01 // Indicates an error occurred. Send a new command to clear it (or nuke it with a Software Reset).

#define MIN(a, b) ((a)<(b)?(a):(b))

PFAT_BPB pBPB = (PFAT_BPB)0x7c00; // same address as bootloader.

uint8_t * FAT_TableDirAddr;

PFAT_DIRENTRY pRootDirList;

VOID InitFAT12Helper()
{
    FAT_TableDirAddr = (unsigned char *)pBPB + pBPB->ReservedSectorCount * pBPB->BytesPerSector;
    pRootDirList = (PFAT_DIRENTRY)
                   (FAT_TableDirAddr + 
                    pBPB->TableCount *
                    pBPB->TableSize *
                    pBPB->BytesPerSector);
}


static BOOL ATAWaitBSY()   // Wait for BSY to be 0. If ERR or DF is set fater BSY is clear, return FALSE. otherwise TRUE.
{
    BYTE Stat;
    do
    {
        Stat = IO_InByte(0x1F7);
    }
    while (Stat & ATA_STATUS_BSY);
    
    if (Stat & (ATA_STATUS_ERR | ATA_STATUS_DF))
        return FALSE;
    return TRUE;
}

static BOOL ATAWaitDRQ()  //Wait fot DRQ to be 1. If ERR or DF is set fater DRQ is set, return FALSE. otherwise TRUE.
{
    BYTE Stat;
    do
    {
        Stat = IO_InByte(0x1F7);
    }
    while (!(Stat & ATA_STATUS_RDY));
    
    if (Stat & (ATA_STATUS_ERR | ATA_STATUS_DF))
        return FALSE;
    return TRUE;
}

BOOL ATAReadSectors(int LBA, int SectorCount, unsigned char *Buffer, int MaxReadSize)
{
    // using ATA PIO mode to read disk
    // see https://wiki.osdev.org/ATA_PIO_Mode#x86_Directions for more details.
    static int LBA2SectorOffset = 0;
    if(LBA2SectorOffset == 0)
    {   
        LBA2SectorOffset = 
            pBPB->ReservedSectorCount                                            // boot sector (or so...)
          + pBPB->TableSize * pBPB->TableCount                                   // FAT Table
          + pBPB->RootEntryCount * sizeof(FAT_DIRENTRY) / pBPB->BytesPerSector   // Root directory
          - 2;                                                                   // FAT number ID from 2. 0 and 1 are not used.
    }
    
    LBA += LBA2SectorOffset;

    if (!ATAWaitBSY()) return FALSE;

    IO_OutByte(0x1F6, 0xE0 | ((LBA >> 24) & 0xF));
    IO_OutByte(0x1F2, SectorCount);
    IO_OutByte(0x1F3, (uint8_t) LBA);
    IO_OutByte(0x1F4, (uint8_t)(LBA >> 8));
    IO_OutByte(0x1F5, (uint8_t)(LBA >> 16)); 
    IO_OutByte(0x1F7, 0x20); // Send the read command

    for (int j = 0; j < SectorCount; j++)
    {
        if (!ATAWaitBSY()) return FALSE;
        if (!ATAWaitDRQ()) return FALSE;

        for (unsigned int i = 0; i < pBPB->BytesPerSector / sizeof(uint16_t); i++)
        {
            if (MaxReadSize >= 2)
            {
                *((short *)(Buffer + i * 2)) = IO_InWord(0x1F0);
                MaxReadSize -= 2;
            }
            else if (MaxReadSize == 1)
            {
                short ReadTmp = IO_InWord(0x1F0);
                *((char *)(Buffer + i * 2))  = ReadTmp & 0xFF;
                MaxReadSize -= 1;
            }
            else
            {
                IO_InWord(0x1F0);
            }
        }

        Buffer += pBPB->BytesPerSector;
    }

    return TRUE;
}

BOOL FAT12ReadRootDirFile(LPCSTR FileName, PBYTE Buffer, UINT BufferSize)
{
    // currently, only support 8.3 short file name.

    // buildup a filename for searching.
    char CompareFileName[12] = "           ";

    int LastDotPos = -1, FileNameLen;

    // max file name is 11 (including ext). if count the dot, it would be 12. loop until 13 to check if it's too long
    for (FileNameLen = 0; FileName[FileNameLen] && FileNameLen <= 13; FileNameLen++)
    {
        if(FileName[FileNameLen] == '.')
        {
            LastDotPos = FileNameLen;
        }
    }

    if (FileNameLen > 12) // definitely too long
    {
        return FALSE;
    }
    if (LastDotPos == -1)
    {
        LastDotPos = FileNameLen;
    }
    
    if (LastDotPos > 8 || FileNameLen - (LastDotPos + 1) > 3)
    {
        // file name or ext too long.
        return FALSE;
    }

    // copy filename and ext.
    for(int i = 0; i < LastDotPos; i++)
    {
        CompareFileName[i] = FileName[i];
    }
    for (int i = LastDotPos + 1; i < FileNameLen; i++)
    {
        CompareFileName[i - LastDotPos - 1 + 8] = FileName[i];
    }

    for(int i = 0; i < pBPB->RootEntryCount; i++)
    {
        BOOL bAllSame = TRUE;
        for(int len = 0; len < 11; len++)
        {
            if (pRootDirList[i].FileName[len] != CompareFileName[len])
            {
                bAllSame = FALSE;
                break;
            }
        }

        if(bAllSame)
        {
            // read file here
            if(pRootDirList[i].Size > BufferSize)
            {
                // Buffer too small
                return FALSE;
            }

            int UnreadSize = pRootDirList[i].Size;
            int Cluster = pRootDirList[i].ClusterLow;

            while (Cluster < 0xFF0 && Cluster > 0x01)
            {
                int Result;
                Result = ATAReadSectors(
                    Cluster,
                    pBPB->SectorsPerCluster,
                    Buffer, 
                    MIN(UnreadSize, pBPB->SectorsPerCluster * pBPB->BytesPerSector));

                if (!Result)
                    return FALSE;

                Buffer += pBPB->SectorsPerCluster * pBPB->BytesPerSector;
                UnreadSize -= pBPB->SectorsPerCluster * pBPB->BytesPerSector;
                unsigned short *NextTableID = (unsigned short *)(FAT_TableDirAddr + Cluster + (Cluster >> 1));
                if(Cluster & 1)
                {
                    Cluster = *(NextTableID) >> 4;
                }
                else
                {
                    Cluster = *(NextTableID) & 0xFFF;
                }
            }
            if (Cluster >= 0xFF8 && Cluster <= 0xFFF)
            {
                // Last Cluster. Successfull read entire file.
                return TRUE;
            }
            // Bad cluster, reserved cluster, or whatever... we've already failed.
            return FALSE;
        }
        // continue searching
    }

    // all searched, non match.
    return FALSE;
}
