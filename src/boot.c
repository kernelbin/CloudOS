/*
 * boot.c   C part code to complete booting work
 * Copyright (c) He yang 2021 (1160386205@qq.com)
 */

#include <stdint.h>
#include "vbedef.h"
#include "MemLayout.h"
#include "font.h"
#include "StringFormat.h"
#include "FATDef.h"


extern VBE_MODE_INFO_STRUCTURE VbeModeInfo;

// Paint a "Desktop" for fun.
VOID PaintDesktop()
{
    PBYTE ScrnBuffer = (PBYTE)VbeModeInfo.framebuffer;

    UINT BytesPerPixel = (VbeModeInfo.bpp >> 3);
    UINT RedOffset =     (VbeModeInfo.red_position >> 3);
    UINT GreenOffset =   (VbeModeInfo.green_position >> 3);
    UINT BlueOffset =    (VbeModeInfo.blue_position >> 3);

    for(INT i = 0; i < VbeModeInfo.height; i++)
    {
        for(INT j = 0; j < VbeModeInfo.width; j++)
        {
            PBYTE PixelAddr = ScrnBuffer + BytesPerPixel * (i * VbeModeInfo.width + j);
            *(PixelAddr + BlueOffset) = 158 * ((double) j / (double) VbeModeInfo.width) + 20;
            *(PixelAddr + GreenOffset) = -138 * ((double) i / (double) VbeModeInfo.height) + 180;
            *(PixelAddr + RedOffset) = (-40 * ((double) j / (double) VbeModeInfo.width) + 60) + 50 * ((double) i / (double) VbeModeInfo.height) -45;
        }
    }
}

INT CloudMain()
{
    PaintDesktop();
    PrepareFont();

    InitFAT12Helper();

    PBYTE Text = (PBYTE)0x100000;

    FAT12ReadRootDirFile("THEEND.TXT", Text, 8651);

    PrintString(23, 23, (char *)Text, 2, 0, 0, 0);
    PrintString(20, 20, (char *)Text, 2, 200, 200, 255);

    CHAR Buf[100] = { 0 };
    StringFormat(Buf,
                 (sizeof(Buf)/sizeof(Buf[0])) - 1,
                 5,
                 TYPE_STR, "Screen Resolution: (",
                 TYPE_INT, VbeModeInfo.width,
                 TYPE_STR, ", ",
                 TYPE_INT, VbeModeInfo.height,
                 TYPE_STR, ")");

    PrintString(23, 53, Buf, 3, 0, 0, 0);
    PrintString(20, 50, Buf, 3, 255, 255, 255);

    INT len = StringFormat(Buf,
                (sizeof(Buf)/sizeof(Buf[0])) - 1,
                3,
                TYPE_STR, "MemInfoEntryCnt = ",
                TYPE_INT, MemInfoEntryCnt,
                TYPE_STR, ".");
    Buf[len] = 0;
    PrintString(23, 83, Buf, 3, 0, 0, 0);
    PrintString(20, 80, Buf, 3, 255, 255, 255);

    for(INT i = 0; i < MemInfoEntryCnt; i++)
    {
        INT len = StringFormat(Buf,
                (sizeof(Buf)/sizeof(Buf[0])) - 1,
                10,
                TYPE_STR, "Base: ",
                TYPE_PTR, MemInfoAddr[i].BaseH,
                TYPE_STR, " ",
                TYPE_PTR, MemInfoAddr[i].BaseL,
                TYPE_STR, " Length: ",
                TYPE_PTR, MemInfoAddr[i].LengthH,
                TYPE_STR, " ",
                TYPE_PTR, MemInfoAddr[i].LengthL,
                TYPE_STR, " -> ",
                TYPE_INT, MemInfoAddr[i].Type
                );
            Buf[len] = 0;
            PrintString(22, 132 + i * 20, Buf, 2, 0, 0, 0);
            PrintString(20, 130 + i * 20, Buf, 2, 255, 255, 255);
    }

    while(1)
    {
        asm("HLT");
    }
    return 0;
}
