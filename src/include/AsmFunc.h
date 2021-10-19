/*
 * MemLayout.h   Assembly function declarations needed by C program.
 * Copyright (c) He yang 2021 (1160386205@qq.com)
 */


#ifndef _ASMFUNC_H_
#define _ASMFUNC_H_

#include <stdint.h>
#define uint16_t unsigned short
#define uint8_t unsigned char
#define uint32_t unsigned int

void __attribute__((__cdecl__)) IO_OutByte(uint16_t port,uint8_t data);
void __attribute__((__cdecl__)) IO_OutWord(uint16_t port,uint16_t data);
uint8_t __attribute__((__cdecl__)) IO_InByte(uint16_t port);
uint16_t __attribute__((__cdecl__)) IO_InWord(uint16_t port);
void __attribute__((__cdecl__)) IO_RepInSWord(uint16_t port, uint32_t buffer, uint32_t count);

#endif // _ASMFUNC_H_
