; fat12def.inc  macros for VBE (VESA BIOS Extension)
; Copyright (c) He yang 2020 (1160386205@qq.com)

; see these structure definition at https://wiki.osdev.org/VESA_Video_Modes

STRUC   VBE_INFO_BLOCK

    .VbeSignature:   RESB    4
    .VbeVersion:     RESW    1
    .OemStringPtr:   RESW    2
    .Capabilities:   RESB    4
    .VideoModePtr:   RESW    2
    .TotalMemory:    RESW    1

ENDSTRUC


STRUC   VBE_MODE_INFO

    .attributes:            RESW 1      ; deprecated, only bit 7 should be of interest to you, and it indicates the mode supports a linear frame buffer.
    .window_a:              RESB 1      ; deprecated
    .window_b:              RESB 1      ; deprecated
    .granularity:           RESW 1      ; deprecated; used while calculating bank numbers
    .window_size:           RESW 1
    .segment_a:             RESW 1
    .segment_b:             RESW 1
    .win_func_ptr:          RESD 1      ; deprecated; used to switch banks from protected mode without returning to real mode
    .pitch:                 RESW 1      ; number of bytes per horizontal line
    .width:                 RESW 1      ; width in pixels
    .height:                RESW 1      ; height in pixels
    .w_char:                RESB 1      ; unused...
    .y_char:                RESB 1      ; ...
    .planes:                RESB 1
    .bpp:                   RESB 1      ; bits per pixel in this mode
    .banks:                 RESB 1      ; deprecated; total number of banks in this mode
    .memory_model:          RESB 1
    .bank_size:             RESB 1      ; deprecated; size of a bank, almost always 64 KB but may be 16 KB...
    .image_pages:           RESB 1
    .reserved0:             RESB 1

    .red_mask:              RESB 1
    .red_position:          RESB 1
    .green_mask:            RESB 1
    .green_position:        RESB 1
    .blue_mask:             RESB 1
    .blue_position:         RESB 1
    .reserved_mask:         RESB 1
    .reserved_position:     RESB 1
    .direct_color_attr:     RESB 1

    .framebuffer:           RESD 1      ; physical address of the linear frame buffer; write here to draw to the screen
    .off_screen_mem_off:    RESD 1
    .off_screen_mem_size:   RESW 1      ; size of memory in the framebuffer but not being displayed on the screen
    .reserved1:             RESB 206

ENDSTRUC
