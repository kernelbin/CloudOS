#include <stdarg.h>
#include "StringFormat.h"

static CHAR HexToChar(INT Hex)
{
    if (Hex < 10)
    {
        return Hex + '0';
    }
    return Hex - 10 + 'A';
}

INT VStringFormat(LPSTR Buffer, INT BufSize, UINT cnt, va_list ValList)
{
    if (Buffer)
    {
        // verify if the size is enough
        INT RequiredSize = VStringFormat(0, 0, cnt, ValList);
        if (RequiredSize == -1) return -1;
        if (RequiredSize > BufSize)
        {
            return 0;
        }
    }
    INT cchLength = 0;
    for (UINT i = 0; i < cnt; i++)
    {
        INT Type = va_arg(ValList, INT);
        switch (Type)
        {
            case TYPE_INT:
            {
                INT Num = va_arg(ValList, INT);
                if (Num < 0)
                {
                    Num = -Num;
                    if (Buffer)
                    {
                        Buffer[cchLength] = '-';
                    }
                    cchLength++;
                }
                INT sp = 0;
                CHAR Stack[10]; // at most 10 digit (demical) for int

                do
                {
                    Stack[sp++] = Num % 10;
                    Num /= 10;
                } while (Num);

                while (sp--)
                {
                    if (Buffer)
                    {
                        Buffer[cchLength] = Stack[sp] + '0';
                    }
                    cchLength++;
                }
                break;
            }
            case TYPE_STR:
            {
                LPCSTR Str = va_arg(ValList, LPCSTR);
                for (UINT i = 0; Str[i]; i++)
                {
                    if (Buffer)
                    {
                        Buffer[cchLength] = Str[i];
                    }
                    cchLength++;
                }
                break;
            }
            case TYPE_PTR:
            {
                INT_PTR PtrInt = va_arg(ValList, INT_PTR);

                if (Buffer)
                {
                    Buffer[cchLength] = '0';
                    Buffer[cchLength + 1] = 'x';
                }
                cchLength += 2;
                for(UINT i = 0; i < sizeof(INT_PTR) * 2; i++)
                {
                    if(Buffer)
                    {
                        Buffer[cchLength + 7 - i] = HexToChar(PtrInt & 0x0000000F);
                        PtrInt >>= 4;
                    }
                }
                cchLength += 8;
                break;
            }
            default:
            {
                return -1;
            }
        }
    }
    return cchLength;
}

INT StringFormat(LPSTR Buffer, INT BufSize, UINT cnt, ...)
{
    va_list ValList;
    va_start(ValList, cnt);

    INT iRet = VStringFormat(Buffer, BufSize, cnt, ValList);
    
    va_end(ValList);
    return iRet;
}
