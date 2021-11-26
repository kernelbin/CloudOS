/*
 * MemLayout.h   Structs about memory layout.
 * Copyright (c) He yang 2021 (1160386205@qq.com)
 */

#include "DataType.h"

// Memory layout information is read in and stored using INT 0x15, EAX = 0xE820 assembly (code in boot.asm)
// These structs are only for accessing it.
// See https://wiki.osdev.org/Detecting_Memory_(x86) for more details.

#ifndef _MEM_LAYOUT_H_
#define _MEM_LAYOUT_H_

typedef struct _MemLayoutEntry {
 
    UINT32 BaseL; // base address uint64_t
    UINT32 BaseH;
    UINT32 LengthL; // length uint64_t
    UINT32 LengthH;
    UINT32 Type; // entry Type
    UINT32 ACPI; // extended
 
}__attribute__((packed)) MEM_LAYOUT_ENTRY;


extern INT MemInfoEntryCnt;
extern MEM_LAYOUT_ENTRY *MemInfoAddr;

#endif //_MEM_LAYOUT_H_
