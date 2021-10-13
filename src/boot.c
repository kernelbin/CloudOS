/*
 * boot.c   C part code to complete booting work
 * Copyright (c) He yang 2021 (1160386205@qq.com)
 */

#include <stddef.h>

typedef  unsigned short uint16;
typedef  unsigned char uint8;
typedef  unsigned int uint32;

struct vbe_mode_info_structure {
	uint16 attributes;		// deprecated, only bit 7 should be of interest to you, and it indicates the mode supports a linear frame buffer.
	uint8 window_a;			// deprecated
	uint8 window_b;			// deprecated
	uint16 granularity;		// deprecated; used while calculating bank numbers
	uint16 window_size;
	uint16 segment_a;
	uint16 segment_b;
	uint32 win_func_ptr;		// deprecated; used to switch banks from protected mode without returning to real mode
	uint16 pitch;			// number of bytes per horizontal line
	uint16 width;			// width in pixels
	uint16 height;			// height in pixels
	uint8 w_char;			// unused...
	uint8 y_char;			// ...
	uint8 planes;
	uint8 bpp;			// bits per pixel in this mode
	uint8 banks;			// deprecated; total number of banks in this mode
	uint8 memory_model;
	uint8 bank_size;		// deprecated; size of a bank, almost always 64 KB but may be 16 KB...
	uint8 image_pages;
	uint8 reserved0;
 
	uint8 red_mask;
	uint8 red_position;
	uint8 green_mask;
	uint8 green_position;
	uint8 blue_mask;
	uint8 blue_position;
	uint8 reserved_mask;
	uint8 reserved_position;
	uint8 direct_color_attributes;
 
	uint32 framebuffer;		// physical address of the linear frame buffer; write here to draw to the screen
	uint32 off_screen_mem_off;
	uint16 off_screen_mem_size;	// size of memory in the framebuffer but not being displayed on the screen
	uint8 reserved1[206];
} __attribute__ ((packed));

extern struct vbe_mode_info_structure VbeModeInfo;

extern int CloudMain();


// Paint a picture for fun.

int CloudMain()
{
    char *x = VbeModeInfo.framebuffer;

    for(int i = 0; i < VbeModeInfo.height; i++)
    {
        for(int j = 0; j < VbeModeInfo.width; j++)
        {
            char *addr = x + 3 * (i * VbeModeInfo.width + j);
            *(addr + 0) = 158 * ((double) j / (double) VbeModeInfo.width) + 20;
            *(addr + 1) = -138 * ((double) i / (double) VbeModeInfo.height) + 180;
            *(addr + 2) = (-40 * ((double) j / (double) VbeModeInfo.width) + 60) + 50 * ((double) i / (double) VbeModeInfo.height) -45;
        }
    }
    while(1)
    {
        asm("HLT");
    }
}