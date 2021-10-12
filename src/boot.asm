; boot.asm      prepare things and jump to real mode.
; Copyright (c) He yang 2021 (1160386205@qq.com)


%include "vbedef.inc"
%include "gdtdef.inc"

VESA_VIDEO_MODE EQU 0x118

ORG     0xbe00
[BITS 16]
        ; Pop out the function we passed from ipl.asm
        POP     AX
        MOV     [FuncPutString], AX
        POP     AX
        MOV     [FuncReadFile], AX

        ; ================ Check if we have VBE (VESA BIOS Extension), and select a video mode. ================

        ; Check the existance of VBE
        XOR     AX, AX
        MOV     ES, AX
        MOV     DI, VbeInfoBlock
        MOV     AX, 0x4f00
        INT     0x10
        CMP     AX, 0x004f
        JNE     VBENotAvailable

        ; Check VBE Version
        MOV     AX, [VbeInfoBlock + VBE_INFO_BLOCK.VbeVersion]
        CMP     AX, 0x0200
        JB      VBENotAvailable ; version too low

        ; Check if the mode we selected is supported.
        MOV     DI, VbeModeInfo
        MOV     CX, VESA_VIDEO_MODE
        MOV     AX, 0x4f01
        INT     0x10
        CMP     AX, 0x004f
        JNE     VBENotAvailable

        ; Check color depth
        CMP     BYTE [VbeModeInfo + VBE_MODE_INFO.bpp], 24
        JNE     VBENotAvailable

        ; Select this mode
        MOV     BX ,VESA_VIDEO_MODE | 0x4000 ; If mode makes use of a linear framebuffer, should OR the mode number with 0x4000.
        MOV     AX ,0x4f02
        INT     0x10
        CMP     AX, 0x004f
        JNE     VBENotAvailable


        ; ================ Load GDT Table and jump to 32bit mode ================

        ; We've already properly set GDTR (DS must equal 0, we've set it in ipl.asm already)
        LGDT    [GDTR]

        ; Close the interrupts
        CLI

        ; Open A20 address line.
        ; There are more than one way to achieve this, see https://wiki.osdev.org/A20_Line
        ; Using Fast A20 here, and do it only when necessary
        IN      AL, 0x92
        TEST    AL, 2
        JNZ     SkipA20
        OR      AL, 2
        AND     AL, 0xFE
        OUT     0x92, AL
        SkipA20:

        ; Set the PE (Protect mode Enable) flag in CR0 register
        MOV     EAX, CR0
        OR      EAX, 1
        MOV     CR0, EAX

        ; We're already in protece mode. Do a long jump to set CS properly.
        JMP     dword SelectorGlobalCode:LABEL_SEG_CODE32

VBENotAvailable:
        MOV     SI, szVBENotAvail
        CALL    PutString
        ; Fall through

FinLoop:
        HLT
        JMP     FinLoop

PutString:
; 显示一个字符串并返回。字符串地址存在 SI 中。
        MOV     AL, [SI]
        INC     SI
        CMP     AL, 0
        JNE     PutloopContinue
        RET

PutloopContinue:
        MOV     AH,0x0e     ; 显示字符
        MOV     BX,15       ; 文本颜色
        INT     0x10        ; 调用 BIOS 显示
        JMP     PutString

szVBENotAvail:
        DB      0x0a, 0x0a
        DB      "Error : VBE Not avaliable or version too low."
        DB      0x0a, 0x0a
        DB      0


VbeInfoBlock:
ISTRUC  VBE_INFO_BLOCK
IEND

VbeModeInfo:
ISTRUC  VBE_MODE_INFO
IEND

; GDT Table

LABEL_GDT:              DESCRIPTOR      0,              0,                              0  ; 空描述符
LABEL_DESC_GDATA:       DESCRIPTOR      0,        0xfffff,   DA_32 | DA_DRW | DA_LIMIT_4K  ; 全局不可执行数据段
LABEL_DESC_GCODE:       DESCRIPTOR      0,        0xfffff,   DA_32 | DA_CR  | DA_LIMIT_4K  ; 全局可执行代码段

SelectorGlobalData      equ     LABEL_DESC_GDATA - LABEL_GDT
SelectorGlobalCode      equ     LABEL_DESC_GCODE - LABEL_GDT

GdtLen  equ     $ - LABEL_GDT

GDTR:   ; struct we'll feed it to LGDT
ISTRUC  GDTR_STRUC
        AT GDTR_STRUC.Limit,    DW GdtLen - 1   ; The size of the table in bytes subtracted by 1
        AT GDTR_STRUC.Base,     DD LABEL_GDT    ; DS is always zero. (We've set it to zero in ipl.asm) so OK to write like this
IEND

; Function address
FuncPutString:
        DW      0

FuncReadFile:
        DW      0

; ================ 32 bit code starting from here ================
[BITS   32]

LABEL_SEG_CODE32:

; reload all data segment registers:
        MOV     AX, SelectorGlobalData
        MOV     DS, AX
        MOV     ES, AX
        MOV     FS, AX
        MOV     GS, AX
        MOV     SS, AX

RealModeFin:
        HLT
        JMP RealModeFin
