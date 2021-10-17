; ipl.asm       inital program loader
; Copyright (c) He yang 2020 (1160386205@qq.com)

%include "fat12def.inc"

ORG     0x7c00
JMP     Entry
NOP
; 标准FAT12格式软盘专用的代码 Stand FAT12 format floppy code
        
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

        DB      0x00             ; 中断13的驱动器号
        DB      0x00             ; Windows NT 会使用，否则保留
        DB      0x29             ; 扩展引导标记，必须是 0x28 或者 0x29
        DD      0x00             ; 卷系列号
        DB      "CLOUDOS_FAT"    ; 卷标，11byte
        DB      "FAT12   "       ; 分区文件系统类型名,8 byte

; 用于 LBA 模式读取磁盘数据的数据包
DataPack:
        DB      0x10            ; 该包大小
        DB      0
BlkCnt: DW      0               ; 要读取的区块数。INT 13 指令会将这个值重置为实际读取的区块数。
DstMem: DW      0               ; 目标内存地址
        DW      0               ; 内存页
OrgLBA: DD      0               ; 要读取的 logical block address，48位里的低32位
        DD      0               ; 要读取的 logical block address, 48位里的高32位


Entry:

; 程序主体

        XOR     AX, AX          ; 寄存器初始化
        MOV     SS, AX
        MOV     DS, AX
        MOV     SP, 0xD00       ; This address (0x500 - 0x7BFF) is available from the memory map

        ; 读取 FAT12 的 FAT表 和 根目录

        ; 检查 INT13h 扩展是否支持
        MOV     AH, 0x41
        MOV     BX, 0x55aa
        MOV     DL, 0x80
        INT     0x13

        JNC     LBASupported

        MOV     SI, szLBANotSupported
        CALL    PutString

        JMP     BootFailed

LBASupported:
        ; FAT表有两张（互为备份），每个9扇区，共占18扇区（往上翻，BIOS parameter block里都写着）
        ; 根目录区有 224 个 Entry，每个 Entry 32 字节，共 7168 字节也就是 14 个扇区 （往上翻，BIOS parameter block里都写着）

        ; 所以，要完整读入 FAT表 两张以及根目录，需要 32 个扇区
        ; 我们把这些内容全部读进 0x7e00开始的地方

        MOV     WORD    [BlkCnt], 32
        MOV     WORD    [DstMem], 0x7e00      ; 内存中 0x7c00 - 0x7dff 被 ipl 占据。从 0x7e00 开始放置
        MOV     DWORD   [OrgLBA], 1           ; 磁盘里第一个 block 就是 ipl 没必要再读。

        ; TODO: 将 AH 设为 0x41 调用 INT 13 以检查 LBA 是否启用

        MOV     SI, DataPack    ; address of "disk address packet"
        MOV     AH, 0x42        ; AL is unused
        MOV     DL, 0x80        ; drive number 0 (OR the drive # with 0x80)
        INT     0x13

        ; TODO: 检查实际读取的扇区数量

        ; Call ReadFile to read boot.bin in
        MOV     AX, 0xbe00
        MOV     DI, szBootBinFileName
        CALL    ReadFile

        CMP     AX, 1
        JE      FailedFindBootBin
        CMP     AX, 2
        JE      InterruptedFileSystem
        JA      BootFailed      ; Unknown error
        
        ; Success.
        ; Before jumping, pass function address using stack
        PUSH    ReadFile
        PUSH    PutString

        JMP     0xbe00

; Usage:
;       AX: where the file will be loaded.
;       DI: the file name string (coresponding with 8-3 file name)
;    return:
;       success:        AX = 0
;       file not found: AX = 1
;       fs interrupted: AX = 2
ReadFile:

        ; 磁盘里的内容被我们读进了 0x7e00 开始的内存，加上 FAT 表两张 9*2*512，所以目录表在 0xa200
        MOV     SI, 0xa200
        XOR     CX, CX

FindBootBinLoop:

        CMP     CX, 224     ; 总共 224 个目录
        JE      ReadFileNotFound

        ; 文件名和扩展名正好是前 11 个字节。逐个比较。
        ; TODO: 跳过空目录项，以及长文件名。
        XOR     BX, BX
        JMP     CmpFileName

CmpNextChar:
        INC     BX

CmpFileName:

        CMP     BX, 11
        JE      BootBinFound
        MOV     DX, [SI + BX]
        CMP     DX, [DI + BX]

        JE      CmpNextChar

        ; 文件名并不一致。跳过。
        INC     CX
        ADD     SI, FAT12_DIR_ENTRY_size
        JMP     FindBootBinLoop

BootBinFound:

        ; 枚举FAT表，将文件读入
        MOV     WORD    CX, [SI + FAT12_DIR_ENTRY.FirstCluster]

        ; 将这个扇区读入指定地址，然后找下一个扇区


        MOV     WORD    [BlkCnt], 1           ; 始终一个扇区一个扇区读取

        MOV     DI, AX ; 存目标读取地址

ReadCluster:
        ; 0xFF8-0xFFF means last cluster in a file
        CMP     CX, 0xff8
        JNB     ReadFileSuccess

        ; 0xFF7 means bad cluster
        CMP     CX, 0xff7
        JNB     ReadFileFailed

        ; 0xFF0-0xFF6 means reserved cluster
        CMP     CX, 0xff0
        JNB     ReadFileFailed

        ; 0x000-0x001 should never used. First cluster begins from 2
        CMP     CX, 0x002
        JB      ReadFileFailed

        MOV     WORD    [DstMem], DI

        ; 加 33 - 2 （加 31）得到实际扇区号
        ; 33 是因为IPL、MBR、FAT1、FAT2、根目录占了33个扇区
        ; 2 是因为 0, 1 号在FAT表里是没被用的
        MOV     WORD    [OrgLBA], CX    ; 没 DWORD 寄存器。反正小端不影响所以用了WORD
        ADD     DWORD   [OrgLBA], 31

        MOV     SI, DataPack            ; address of "disk address packet"
        MOV     AH, 0x42                ; AL is unused
        MOV     DL, 0x80                ; drive number 0 (OR the drive # with 0x80)
        INT     0x13
        ; TODO: 校验是否读取成功

        ; 在 FAT 表中找到下一个簇号
        MOV     BX, CX
        SHR     BX, 1
        ADD     BX, CX

        ; move the destination address back
        ADD     DI, 0x0200

        AND     CX, 1
        JNZ     OddCluster
        MOV     WORD    CX, [0x7e00 + BX]
        AND     CX, 0x0fff

        JMP     ReadCluster

OddCluster:
        MOV     WORD    CX, [0x7e00 + BX]
        SHR     CX, 4

        JMP     ReadCluster


ReadFileSuccess:
        MOV     AX, 0
        RET

ReadFileNotFound:
        MOV     AX, 1
        RET

ReadFileFailed:
        MOV     AX, 2
        RET

FailedFindBootBin:
        MOV     SI, szFailedFindBootBin
        CALL    PutString

        JMP     BootFailed

InterruptedFileSystem:
        MOV     SI, szInterruptedFileSystem
        CALL    PutString

        JMP     BootFailed

PutString:
; 显示一个字符串并返回。字符串地址存在 SI 中。
        MOV     AL, [SI]
        INC     SI          ; SI 自增 1
        CMP     AL, 0
        JNE     PutloopContinue
        RET
PutloopContinue:
        MOV     AH,0x0e     ; 显示字符
        MOV     BX,15       ; 文本颜色
        INT     0x10        ; 调用 BIOS 显示
        JMP     PutString

BootFailed:
        MOV     SI, szOSFailedBoot
        CALL    PutString

BootFailedLoop:
        HLT
        JMP     BootFailedLoop     ; 无限循环


; 字符串定义

szFailedFindBootBin: 
        DB      0x0a, 0x0a        ; 换行两次
        DB      "Failed to find BOOT.BIN"
        DB      0x0d, 0x0a         ; CRLF 换行
        DB      0

szLBANotSupported:
        DB      0x0a, 0x0a        ; 换行两次
        DB      "LBA not supported."
        DB      0x0d, 0x0a         ; CRLF 换行
        DB      0

szInterruptedFileSystem:
        DB      0x0a, 0x0a        ; 换行两次
        DB      "Filesystem is interrupted."
        DB      0x0d, 0x0a         ; CRLF 换行
        DB      0

szOSFailedBoot:
        DB      "CloudOS failed to boot."
        DB      0

szBootBinFileName:
        DB      "BOOT    BIN"
        DB      0


        TIMES   510-($-$$)    DB      0

        DB        0x55, 0xaa
