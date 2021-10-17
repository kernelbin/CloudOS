/*
 * font.h    reading/parsing font file.
 * Copyright (c) He yang 2021 (1160386205@qq.com)
 *
 * see these structure definition at https://wiki.osdev.org/VESA_Video_Modes
 */

#ifndef _FONT_H_
#define _FONT_H_

#include <stdint.h>

typedef struct
{
    uint8_t fontHeight;
    uint8_t chWidth[128];
} FONTHEADER;

void PrepareFont();
int GetCharWidth(int ch, int size);
int GetFontHeight(int size);
int PaintChar(int cx, int cy, int ch, int size, int r, int g, int b);
int PrintString(int cx, int cy, char str[], int size, int r, int g, int b);

#endif // _FONT_H_
