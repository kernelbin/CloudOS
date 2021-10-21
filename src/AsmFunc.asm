; AsmFunc.asm   assembly functions needed by C program.
; Copyright (c) He yang 2021 (1160386205@qq.com)


GLOBAL IO_OutByte
GLOBAL IO_OutWord
GLOBAL IO_InByte
GLOBAL IO_InWord
GLOBAL IO_RepInSWord

[BITS 32]

IO_OutByte:    ; void __cdecl _IO_OutByte(uint16_t port,uint8_t data)
        MOV     BYTE AL, [ESP + 8] ; data
        MOV     WORD DX, [ESP + 4] ; port
        OUT     DX, AL
        RET

IO_OutWord:     ; void __cdecl _IO_OutWord(uint16_t port,uint16_t data)
        MOV     WORD AX, [ESP + 8] ; data
        MOV     WORD DX, [ESP + 4] ; port
        OUT     DX, AX
        RET

IO_InByte:      ; uint8_t __cdecl _IO_InByte(uint16_t port)
        MOV     WORD DX, [ESP + 4]; port
        XOR     EAX, EAX
        IN      AL, DX ; ret = AL
        RET

IO_InWord:      ; uint16_t __cdecl _IO_InWord(uint16_t port)
        MOV     WORD DX, [ESP + 4]; port
        XOR     EAX, EAX
        IN      AX, DX ; ret = AX
        RET

IO_RepInSWord:  ; void __cdecl _IO_RepInSWord(uint16_t port, uint32_t buffer, uint32_t count)
        PUSH    EDI
        MOV     WORD DX, [ESP + 8]
        MOV     DWORD EDI, [ESP + 12]
        MOV     DWORD ECX, [ESP + 16]
        CLD
        REP     INSW
        POP     EDI
        RET
