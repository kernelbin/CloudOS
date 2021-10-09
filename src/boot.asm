ORG 0xbe00

MOV     SI, szLoadSuccess
CALL    PutString
JMP     loop


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

szLoadSuccess:
        DB      0x0a, 0x0a        ; 换行两次
        DB      "hello"
        DB      0x0d, 0x0a         ; CRLF 换行
        DB      0

loop:
    HLT
    JMP     loop
