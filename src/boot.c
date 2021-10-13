/*
 * boot.c   C part code to complete booting work
 * Copyright (c) He yang 2021 (1160386205@qq.com)
 */

#include <stdint.h>
#include "vbedef.h"
#include "font.h"

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

    PrintString(20, 20, "CloudOS build time: " __DATE__ " " __TIME__, 2, 255, 255, 255);

    while(1)
    {
        asm("HLT");
    }
}
