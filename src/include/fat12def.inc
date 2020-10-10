; fat12def.inc     macros for FAT12 file system
; Copyright (c) He yang 2020 (1160386205@qq.com)

STRUC   FAT12_DIR_ENTRY

    .FileName       RESB    8
    .ExtName        RESB    3
    .FileAttr       RESB    1
    .Reserved       RESB    10
    .CreationTime   RESW    1
    .CreationDate   RESW    1
    .FirstCluster   RESW    1
    .FileSize       RESD    1

ENDSTRUC
    