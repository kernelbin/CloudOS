; ipl.asm     inital program loader
; Copyright (c) He yang 2020 (1160386205@qq.com)


org 0x7c00
jmp Entry

; 标准FAT12格式软盘专用的代码 Stand FAT12 format floppy code

        DB      0x90
        
; 有关这段 BIOS parameter block
; 更多信息详见 https://wiki.osdev.org/FAT

        DB      "CLOUDIPL"       ; 启动扇区名称（8字节）
        DW      512              ; 每个扇区（sector）大小（必须512字节）
        DB      1                ; 簇（cluster）大小（必须为1个扇区）
        DW      1                ; FAT起始位置（一般为第一个扇区）
        DB      2                ; FAT个数（必须为2）
        DW      224              ; 根目录大小（一般为224项）
        DW      2880             ; 该磁盘大小（必须为2880扇区1440*1024/512）
        DB      0xf0             ; 磁盘类型（必须为0xf0，见 https://infogalactic.com/info/Design_of_the_FAT_file_system#media ）
        DW      9                ; FAT的长度（必须是9扇区）
        DW      18               ; 一个磁道（track）有几个扇区（必须为18）
        DW      2                ; 磁头数（必须是2）
        DD      0                ; 不使用分区，必须是0
        DD      2880             ; 重写一次磁盘大小
        DB      0,0,0x29         ; 意义不明（固定）
        DD      0xffffffff       ; （可能是）卷标号码
        DB      "CLOUD - OS "    ; 磁盘的名称（必须为11字节，不足填空格）
        DB      "FAT12   "       ; 磁盘格式名称（必须是8字节，不足填空格）

Entry:

; 程序主体

        MOV     AX, 0               ; 寄存器初始化
        MOV     SS, AX
        MOV     DS, AX
        MOV     SP, 0x7c00

        ; 输出 szLoadingKernel 在屏幕上
        MOV     SI, szLoadingKernel
        CALL    PutString

        JMP     bootfin


PutString:
; 显示一个字符串并返回。字符串地址存在 SI 中。
        MOV     AL,[SI]
        ADD     SI,1        ; SI 自增 1
        CMP     AL,0
        JNE     putloop_continue
        RET 
putloop_continue:
        MOV     AH,0x0e     ; 显示字符
        MOV     BX,15       ; 文本颜色
        INT     0x10        ; 调用 BIOS 显示
        JMP     PutString

bootfin:
		HLT						
		JMP		bootfin				; 无限循环


; 字符串定义

szLoadingKernel:
        DB        0x0a, 0x0a        ; 换行两次
        DB        "Loading kernel file..."
        DB        0x0d 0x0a         ; CRLF 换行
        DB        0

        times     510-($-$$)    DB    00

        DB        0x55, 0xaa

; 启动扇区以外部分输出

        DB      0xf0, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00
        TIMES   4600    DB  0
        DB      0xf0, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00
        TIMES   1469432 DB  0