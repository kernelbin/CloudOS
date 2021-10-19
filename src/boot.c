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
void PaintDesktop()
{
    char *ScrnBuffer = (char *)VbeModeInfo.framebuffer;

    for(int i = 0; i < VbeModeInfo.height; i++)
    {
        for(int j = 0; j < VbeModeInfo.width; j++)
        {
            char *PixelAddr = ScrnBuffer + 3 * (i * VbeModeInfo.width + j);
            *(PixelAddr + 0) = 158 * ((double) j / (double) VbeModeInfo.width) + 20;
            *(PixelAddr + 1) = -138 * ((double) i / (double) VbeModeInfo.height) + 180;
            *(PixelAddr + 2) = (-40 * ((double) j / (double) VbeModeInfo.width) + 60) + 50 * ((double) i / (double) VbeModeInfo.height) -45;
        }
    }
}

int CloudMain()
{
    PaintDesktop();
    PrepareFont();

    InitFAT12Helper();

    unsigned char* Text = (unsigned char*)0x100000;

    FAT12ReadRootDirFile("THEEND.TXT", Text, 8651);

    PrintString(23, 23, (char *)Text, 2, 0, 0, 0);
    PrintString(20, 20, (char *)Text, 2, 200, 200, 255);

    char Buf[100] = { 0 };
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

    int len = StringFormat(Buf,
                (sizeof(Buf)/sizeof(Buf[0])) - 1,
                3,
                TYPE_STR, "MemInfoEntryCnt = ",
                TYPE_INT, MemInfoEntryCnt,
                TYPE_STR, ".");
    Buf[len] = 0;
    PrintString(23, 83, Buf, 3, 0, 0, 0);
    PrintString(20, 80, Buf, 3, 255, 255, 255);

    for(int i = 0; i < MemInfoEntryCnt; i++)
    {
        int len = StringFormat(Buf,
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
