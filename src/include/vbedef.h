/*
 * vbedef.h   structures for VBE (VESA BIOS Extension)
 * Copyright (c) He yang 2021 (1160386205@qq.com)
 *
 * see these structure definition at https://wiki.osdev.org/VESA_Video_Modes
 */

#ifndef _VBEDEF_H_
#define _VBEDEF_H_

#include "DataType.h"

typedef struct _VBE_MODE_INFO_STRUCTURE {
    WORD    attributes;     // deprecated, only bit 7 should be of interest to you, and it indicates the mode supports a linear frame buffer.
    BYTE    window_a;       // deprecated
    BYTE    window_b;       // deprecated
    WORD    granularity;    // deprecated; used while calculating bank numbers
    WORD    window_size;
    WORD    segment_a;
    WORD    segment_b;
    LPVOID  win_func_ptr;   // deprecated; used to switch banks from protected mode without returning to real mode
    WORD    pitch;          // number of bytes per horizontal line
    WORD    width;          // width in pixels
    WORD    height;         // height in pixels
    BYTE    w_char;         // unused...
    BYTE    y_char;         // ...
    BYTE    planes;
    INT8    bpp;            // bits per pixel in this mode
    BYTE    banks;          // deprecated; total number of banks in this mode
    BYTE    memory_model;
    BYTE    bank_size;      // deprecated; size of a bank, almost always 64 KB but may be 16 KB...
    BYTE    image_pages;
    BYTE    reserved0;
 
    BYTE    red_mask;       // size of direct color mask in bits
    UINT8   red_position;   // bit position (least significant bit)
    BYTE    green_mask;     // same above
    UINT8   green_position; // same above
    BYTE    blue_mask;      // same above
    UINT8   blue_position;  // same above
    BYTE    reserved_mask;
    UINT8   reserved_position;
    BYTE    direct_color_attributes;
 
    LPVOID  framebuffer;            // physical address of the linear frame buffer; write here to draw to the screen
    DWORD   off_screen_mem_off;
    UINT16  off_screen_mem_size;    // size of memory in the framebuffer but not being displayed on the screen
    BYTE    reserved1[206];
} __attribute__ ((packed)) VBE_MODE_INFO_STRUCTURE;

#endif // _VBEDEF_H_
