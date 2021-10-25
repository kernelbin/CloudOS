/*
 * font.c    reading/parsing font file.
 * Copyright (c) He yang 2021 (1160386205@qq.com)
 *
 * see these structure definition at https://wiki.osdev.org/VESA_Video_Modes
 */

#include <stdint.h>
#include "font.h"
#include "vbedef.h"

#define _countof(a) (sizeof(a)/sizeof(a[0]))

extern FONTHEADER* FontFileAddr;
extern VBE_MODE_INFO_STRUCTURE VbeModeInfo;

static int CharOffset[_countof(FontFileAddr->chWidth)];

void PrepareFont()
{
    // calculate the offset of each char

    CharOffset[0] = 0;
    for(uint32_t i = 1; i < _countof(FontFileAddr->chWidth); i++)
    {
        CharOffset[i] = CharOffset[i - 1] + FontFileAddr->chWidth[i - 1] * FontFileAddr->fontHeight;
    }
}

int GetCharWidth(int ch, int size)
{
    return FontFileAddr->chWidth[ch] * size;
}

int GetFontHeight(int size)
{
    return FontFileAddr->fontHeight * size;
}

int PaintChar(int cx, int cy, int ch, int size, int r, int g, int b)
{
    // TODO: This function is directly reading from video memory, which could be VERY SLOW.
    // Use double buffering instead.

    // TODO: boundary checking is missing.

    if (ch > (int)_countof(FontFileAddr->chWidth)) return 0;

    int PixelOffset = 0;
    unsigned char *FontContent = (unsigned char *)FontFileAddr;
    FontContent += sizeof(FONTHEADER) + CharOffset[ch];

    unsigned char *ScrnBuffer = (unsigned char *)VbeModeInfo.framebuffer;
    int BytesPerPixel = (VbeModeInfo.bpp >> 3);
    int RedOffset =     (VbeModeInfo.red_position >> 3);
    int GreenOffset =   (VbeModeInfo.green_position >> 3);
    int BlueOffset =    (VbeModeInfo.blue_position >> 3);

    for (int i = 0; i < FontFileAddr->fontHeight; i++)
    {
        for (int j = 0; j < FontFileAddr->chWidth[ch]; j++)
        {
            for (int y = 0; y < size; y++)
            {
                for(int x = 0; x < size; x++)
                {
                    unsigned char *PixelAddr = ScrnBuffer + BytesPerPixel * ((cy + i * size + y) * VbeModeInfo.width + (cx + j * size + x));
                    int pxFont = *(FontContent + PixelOffset);
                    
                    *(PixelAddr + BlueOffset)  = (*(PixelAddr + BlueOffset)  * pxFont + b * (255 - pxFont)) / 255;
                    *(PixelAddr + GreenOffset) = (*(PixelAddr + GreenOffset) * pxFont + g * (255 - pxFont)) / 255;
                    *(PixelAddr + RedOffset)   = (*(PixelAddr + RedOffset)   * pxFont + r * (255 - pxFont)) / 255;
                }
            }
            PixelOffset++;
        }
    }
    return  0;
}

int PrintString(int cx, int cy, char str[], int size, int r, int g, int b)
{
    // TODO: boundary checking is missing.
    
    for(int i = 0; str[i]; i++)
    {
        PaintChar(cx, cy, str[i], size, r, g, b);
        cx += GetCharWidth(str[i], size) + size / 2;
    }
    return 0;
}
