/*
 * MemLayout.h   Structs about memory layout.
 * Copyright (c) He yang 2021 (1160386205@qq.com)
 */


// Memory layout information is read in and stored using INT 0x15, EAX = 0xE820 assembly (code in boot.asm)
// These structs are only for accessing it.
// See https://wiki.osdev.org/Detecting_Memory_(x86) for more details.

#ifndef _MEM_LAYOUT_H_
#define _MEM_LAYOUT_H_

 typedef struct _MemLayoutEntry {
 
    uint32_t BaseL; // base address uint64_t
    uint32_t BaseH;
    uint32_t LengthL; // length uint64_t
    uint32_t LengthH;
    uint32_t Type; // entry Type
    uint32_t ACPI; // extended
 
}__attribute__((packed)) MEM_LAYOUT_ENTRY;


extern int MemInfoEntryCnt;
extern MEM_LAYOUT_ENTRY *MemInfoAddr;

#endif //_MEM_LAYOUT_H_
