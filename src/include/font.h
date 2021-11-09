/*
 * font.h    reading/parsing font file.
 * Copyright (c) He yang 2021 (1160386205@qq.com)
 *
 * see these structure definition at https://wiki.osdev.org/VESA_Video_Modes
 */

#ifndef _FONT_H_
#define _FONT_H_

#include "DataType.h"

typedef struct
{
    UINT8   fontHeight;
    UINT8   chWidth[128];
} FONTHEADER;

VOID PrepareFont();
INT GetCharWidth(CHAR ch, INT size);
INT GetFontHeight(INT size);
BOOL PaintChar(INT cx, INT cy, CHAR ch, INT size, BYTE r, BYTE g, BYTE b);
BOOL PrintString(INT cx, INT cy, LPCSTR str, INT size, BYTE r, BYTE g, BYTE b);

#endif // _FONT_H_
