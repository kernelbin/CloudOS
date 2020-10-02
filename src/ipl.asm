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

; 用于 LBA 模式读取磁盘数据的数据包
DAPACK:
        db      0x10            ; 该包大小
        db      0
BlkCnt:	dw      0               ; 要读取的区块数。INT 13 指令会将这个值重置为实际读取的区块数。
DstMem:	dw      0               ; 目标内存地址
        dw      0               ; 内存页
OrgLBA: dd      0               ; 要读取的 logical block address，48位里的低32位
        dd      0               ; 要读取的 logical block address, 48位里的高32位



Entry:

; 程序主体

        MOV     AX, 0               ; 寄存器初始化
        MOV     SS, AX
        MOV     DS, AX
        MOV     SP, 0x7c00

        ; 输出 szLoadingKernel 在屏幕上
        MOV     SI, szLoadingKernel
        CALL    PutString



; 读取 FAT12 的 FAT表 和 根目录

        ; FAT表有两张（互为备份），每个9扇区，共占18扇区（往上翻，BIOS parameter block里都写着）
        ; 根目录区有 224 个 Entry，每个 Entry 32 字节，共 7168 字节也就是 14 个扇区 （往上翻，BIOS parameter block里都写着）

        ; 所以，要完整读入 FAT表 两张以及根目录，需要 32 个扇区
        ; 我们把这些内容全部读进 0x7e00开始的地方
        
        MOV     WORD    [BlkCnt], 32
        MOV     WORD    [DstMem], 0x7e00
        MOV     DWORD   [OrgLBA], 1

        ; TODO: 将 AH 设为 0x41 调用 INT 13 以检查 LBA 是否启用

        MOV     SI, DAPACK      ; address of "disk address packet"
        mov     AH, 0x42        ; AL is unused
        mov     DL, 0x80        ; drive number 0 (OR the drive # with 0x80)
        INT     0x13

        ; TODO: 检查实际读取的扇区数量
        
        ; TODO: 在 FAT 文件系统中寻找文件 boot.bin 并且读入内存 0xbe00，然后跳转过去
        
        JMP bootfin

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
        DB        0x0d, 0x0a         ; CRLF 换行
        DB        0

        times     510-($-$$)    DB    00

        DB        0x55, 0xaa