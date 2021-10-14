#include <stdarg.h>
#include "StringFormat.h"

char HexToChar(int Hex)
{
    if (Hex < 10)
    {
        return Hex + '0';
    }
    return Hex - 10 + 'A';
}

int VStringFormat(char Buffer[], int BufSize, int cnt, va_list ValList)
{
    if (Buffer)
    {
        // verify if the size is enough
        int RequiredSize = VStringFormat(0, 0, cnt, ValList);
        if (RequiredSize == -1) return -1;
        if (RequiredSize > BufSize)
        {
            return 0;
        }
    }
    int cchLength = 0;
    for (int i = 0; i < cnt; i++)
    {
        int Type = va_arg(ValList, int);
        switch (Type)
        {
            case TYPE_INT:
            {
                int Num = va_arg(ValList, int);
                if (Num < 0)
                {
                    Num = -Num;
                    if (Buffer)
                    {
                        Buffer[cchLength] = '-';
                    }
                    cchLength++;
                }
                int sp = 0;
                char Stack[10]; // at most 10 digit (demical) for int

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
                char* Str = va_arg(ValList, char*);
                for (int i = 0; Str[i]; i++)
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
                unsigned int PtrInt = va_arg(ValList, unsigned int);

                if (Buffer)
                {
                    Buffer[cchLength] = '0';
                    Buffer[cchLength + 1] = 'x';
                }
                cchLength += 2;
                for(int i = 0; i < 8; i++)
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

int StringFormat(char Buffer[], int BufSize, int cnt, ...)
{
    va_list ValList;
    va_start(ValList, cnt);

    int iRet = VStringFormat(Buffer, BufSize, cnt, ValList);
    
    va_end(ValList);
    return iRet;
}
