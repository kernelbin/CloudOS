; boot.asm      prepare things and jump to real mode.
; Copyright (c) He yang 2021 (1160386205@qq.com)


%include "vbedef.inc"
%include "gdtdef.inc"

global VbeModeInfo
global FontFileAddr
global MemInfoEntryCnt
global MemInfoAddr

extern CloudMain

VESA_VIDEO_MODE EQU 0x118

[SECTION .text16]
[BITS 16]
        ; Pop out the function we passed from ipl.asm
        POP     AX
        MOV     [FuncPutString], AX
        POP     AX
        MOV     [FuncReadFile], AX

        ; Get Memory Layout Information
        MOV     AX, 0
        MOV     ES, AX
        MOV     DI, 0
        call    GetMemoryLayout
        JC      FailedGetMemoryLayout

        ; Load Font File
        MOV     AX, [FontFileAddr]
        MOV     DI, FontFileName
        CALL    [FuncReadFile]

        CMP     AX, 1
        JE      FailedFindFont
        CMP     AX, 2
        JE      InterruptedFileSystem
        JA      BootFailed      ; Unknown error

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
        JMP     DWORD SelectorGlobalCode:LABEL_SEG_CODE32

; use the INT 0x15, EAX = 0xE820 BIOS function to get a memory map
; note: initially di is 0, be sure to set it to a value so that the BIOS code will not be overwritten.
;       The consequence of overwriting the BIOS code will lead to problems like getting stuck in `int 0x15`
; inputs: ES:DI -> destination buffer for 24 byte entries
; outputs: entry count result will be stored in [MemInfoEntryCnt], and the where the result is stored at address [MemInfoAddr]
;          carry flag will be set if failed.
GetMemoryLayout:
        MOV     AX, [MemInfoAddr]
        MOV     DI, AX
        XOR     EBX, EBX                ; EBX must be 0 to start
        XOR     BP, BP                  ; keep an entry count in BP
        MOV     EDX, 0x0534D4150        ; Place "SMAP" into EDX
        MOV     EAX, 0xE820
        MOV     [ES:DI + 20], DWORD 1   ; force a valid ACPI 3.X entry
        MOV     ECX, 24                 ; ask for 24 bytes
        INT     0x15
        JC      SHORT .GetMemLayoutFailed; carry set on first call means "unsupported function"
        MOV     EDX, 0x0534D4150        ; Some BIOSes apparently trash this register?
        CMP     EAX, EDX                ; on success, EAX must have been reset to "SMAP"
        JNE     SHORT .GetMemLayoutFailed
        TEST    EBX, EBX                ; EBX = 0 implies list is only 1 entry long (worthless)
        JE      SHORT .GetMemLayoutFailed
        JMP     SHORT .JmpIn
.E820LP:
        MOV     EAX, 0xE820             ; EAX, ECX get trashed on every int 0x15 call
        MOV     [ES:DI + 20], DWORD 1   ; force a valid ACPI 3.X entry
        MOV     ECX, 24                 ; ask for 24 bytes again
        INT     0x15
        JC      SHORT .E820F             ; carry set means "end of list already reached"
        MOV     EDX, 0x0534D4150        ; repair potentially trashed register
.JmpIn:
        JCXZ    .SkipEntry               ; skip any 0 length entries
        CMP     CL, 20                  ; got a 24 byte ACPI 3.X response?
        JBE     SHORT .NoText
        TEST    BYTE [ES:DI + 20], 1    ; if so: is the "ignore this data" bit clear?
        JE      SHORT .SkipEntry
.NoText:
        MOV     ECX, [ES:DI + 8]        ; get lower uint32_t of memory region length
        OR      ECX, [ES:DI + 12]       ; "or" it with upper uint32_t to test for zero
        JZ      .SkipEntry               ; if length uint64_t is 0, skip entry
        INC     BP                      ; got a good entry: ++count, move to next storage spot
        ADD     DI, 24
.SkipEntry:
        TEST    EBX, EBX                ; if EBX resets to 0, list is complete
        JNE     SHORT .E820LP
.E820F:
        MOV [MemInfoEntryCnt], BP       ; store the entry count
        CLC                             ; there is "jc" on end of list to this point, so the carry must be cleared
        RET
.GetMemLayoutFailed:
        STC                             ; "function unsupported" error exit
        RET



VBENotAvailable:
        MOV     SI, szVBENotAvail
        CALL    [FuncPutString]
        JMP     BootFailed

FailedGetMemoryLayout:
        MOV     SI, szFailedGetMemoryLayout
        CALL    [FuncPutString]
        JMP     BootFailed

FailedFindFont:
        MOV     SI, szFailedFindFont
        CALL    [FuncPutString]
        JMP     BootFailed

InterruptedFileSystem:
        MOV     SI, szInterruptedFileSystem
        CALL    [FuncPutString]

        JMP     BootFailed

BootFailed:
        HLT
        JMP     BootFailed

szFailedGetMemoryLayout:
        DB      0x0a, 0x0a        ; 换行两次
        DB      "Failed to get memory layout info."
        DB      0x0d, 0x0a         ; CRLF 换行
        DB      0

szFailedFindFont:
        DB      0x0a, 0x0a        ; 换行两次
        DB      "Failed to find Font File"
        DB      0x0d, 0x0a         ; CRLF 换行
        DB      0

szInterruptedFileSystem:
        DB      0x0a, 0x0a        ; 换行两次
        DB      "Filesystem is interrupted."
        DB      0x0d, 0x0a         ; CRLF 换行
        DB      0

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

; Font file name
FontFileName:
        DB      "FONT    FNT"
        DB      0

; Memory layout info address
MemInfoEntryCnt:
        DD      0
MemInfoAddr:
        DD      0xDE00

; Font file address
FontFileAddr:
        DD      0x5C00

; ================ 32 bit code starting from here ================
[SECTION .text32]
[BITS   32]

LABEL_SEG_CODE32:

; reload all data segment registers:
        MOV     AX, SelectorGlobalData
        MOV     DS, AX
        MOV     ES, AX
        MOV     FS, AX
        MOV     GS, AX
        MOV     SS, AX

        JMP     CloudMain