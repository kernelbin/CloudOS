; boot.asm      prepare things and jump to real mode.
; Copyright (c) He yang 2021 (1160386205@qq.com)


%include "vbedef.inc"

VESA_VIDEO_MODE EQU 0x118

ORG     0xbe00

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
        MOV     CX, VESA_VIDEO_MODE
        MOV     AX, 0x4f01
        INT     0x10
        CMP     AX, 0x004f
        JNE     VBENotAvailable

        ; Check color depth
        CMP     BYTE [VbeInfoBlock + VBE_MODE_INFO.bpp], 24
        JNE     VBENotAvailable

        MOV     SI, QWQ
        CALL    PutString
        JMP     FinLoop

QWQ:
DB "All checking passed!"
DB 0

VBENotAvailable:
        MOV     SI, szVBENotAvail
        CALL    PutString
        ; Fall through

FinLoop:
        HLT
        JMP     FinLoop

PutString:
; 显示一个字符串并返回。字符串地址存在 SI 中。
        MOV     AL,[SI]
        ADD     SI,1        ; SI 自增 1
        CMP     AL,0
        JNE     PutloopContinue
        RET
PutloopContinue:
        MOV     AH,0x0e     ; 显示字符
        MOV     BX,15       ; 文本颜色
        INT     0x10        ; 调用 BIOS 显示
        JMP     PutString

VbeInfoBlock:
        ISTRUC VBE_INFO_BLOCK
        IEND

VbeModeInfo:
        ISTRUC VBE_MODE_INFO
        IEND

szVBENotAvail:
        DB      0x0a, 0x0a
        DB      "Error : VBE Not avaliable or version too low."
        DB      0x0a, 0x0a
        DB      0
