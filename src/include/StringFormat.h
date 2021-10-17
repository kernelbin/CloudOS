/*
 * StringFormat.c  Utils for formatting a string.
 * Copyright (c) He yang 2021 (1160386205@qq.com)
 */

#ifndef _STRING_FORMAT_H_
#define _STRING_FORMAT_H_

#include <stdarg.h>

#define TYPE_INT 1
#define TYPE_STR 2
#define TYPE_PTR 3

int VStringFormat(char Buffer[], int BufSize, int cnt, va_list ValList);
int StringFormat(char Buffer[], int BufSize, int cnt, ...);

#endif // _STRING_FORMAT_H_
