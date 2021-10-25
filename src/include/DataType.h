/*
 * DataType.h    Data Type defines
 * Copyright (c) He yang 2021 (1160386205@qq.com)
 *
 */

#ifndef _DATA_TYPE_H_
#define _DATA_TYPE_H_

#include <stdint.h>

#define VOID                void
#define CONST               const

typedef char                CHAR;
typedef short               SHORT;
typedef long                LONG;
typedef int                 INT;
typedef unsigned int        DWORD;
typedef int                 BOOL;
typedef unsigned char       BYTE;
typedef unsigned short      WORD;
typedef float               FLOAT;
typedef FLOAT               *PFLOAT;
typedef BOOL                *PBOOL;
typedef BOOL                *LPBOOL;
typedef BYTE                *PBYTE;
typedef BYTE                *LPBYTE;
typedef int                 *PINT;
typedef int                 *LPINT;
typedef WORD                *PWORD;
typedef WORD                *LPWORD;
typedef long                *LPLONG;
typedef DWORD               *PDWORD;
typedef DWORD               *LPDWORD;
typedef void                *LPVOID;
typedef CONST void          *LPCVOID;

typedef int                 INT;
typedef unsigned int        UINT;
typedef unsigned int        *PUINT;

typedef signed char         INT8, * PINT8;
typedef signed short        INT16, * PINT16;
typedef signed int          INT32, * PINT32;
typedef unsigned char       UINT8, * PUINT8;
typedef unsigned short      UINT16, * PUINT16;
typedef unsigned int        UINT32, * PUINT32;

_Static_assert(sizeof(char)  == 1, "Size of char is not 1");
_Static_assert(sizeof(short) == 2, "Size of short is not 2");
_Static_assert(sizeof(int)   == 4, "Size of int is not 4");

#endif // _DATA_TYPE_H_
