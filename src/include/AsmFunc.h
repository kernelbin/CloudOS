/*
 * MemLayout.h   Assembly function declarations needed by C program.
 * Copyright (c) He yang 2021 (1160386205@qq.com)
 */


#ifndef _ASMFUNC_H_
#define _ASMFUNC_H_

#include "DataType.h"

VOID __attribute__((__cdecl__)) IO_OutByte(WORD port,BYTE data);
VOID __attribute__((__cdecl__)) IO_OutWord(WORD port,WORD data);
BYTE __attribute__((__cdecl__)) IO_InByte(WORD port);
WORD __attribute__((__cdecl__)) IO_InWord(WORD port);
VOID __attribute__((__cdecl__)) IO_RepInSWord(WORD port, LPBYTE buffer, UINT count);

#endif // _ASMFUNC_H_
