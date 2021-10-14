/*
 * boot.c   C part code to complete booting work
 * Copyright (c) He yang 2021 (1160386205@qq.com)
 */

#include <stdint.h>
#include "vbedef.h"
#include "font.h"
#include "StringFormat.h"

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

    char Buf[100] = { 0 };
    StringFormat(Buf,
                 (sizeof(Buf)/sizeof(Buf[0])) - 1,
                 5,
                 TYPE_STR, "Screen Resolution: (",
                 TYPE_INT, VbeModeInfo.width,
                 TYPE_STR, ", ",
                 TYPE_INT, VbeModeInfo.height,
                 TYPE_STR, ")");

    PrintString(23, 23, Buf, 3, 0, 0, 0);
    PrintString(20, 20, Buf, 3, 255, 255, 255);

    while(1)
    {
        asm("HLT");
    }
}
