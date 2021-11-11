/*
 * StringFormat.c  Utils for formatting a string.
 * Copyright (c) He yang 2021 (1160386205@qq.com)
 */

#ifndef _STRING_FORMAT_H_
#define _STRING_FORMAT_H_

#include <stdarg.h>
#include "DataType.h"

#define TYPE_INT 1
#define TYPE_STR 2
#define TYPE_PTR 3

INT VStringFormat(LPSTR Buffer, INT BufSize, UINT cnt, va_list ValList);
INT StringFormat(LPSTR Buffer, INT BufSize, UINT cnt, ...);

#endif // _STRING_FORMAT_H_
