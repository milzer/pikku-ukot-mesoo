// Win32 types and MSVC CRT extras the game code picked up through <windows.h>
// and the Diesel headers. Sized explicitly so pixel buffers and saved files keep
// their 32-bit layout on 64-bit platforms.

#ifndef __WINCOMPAT_H__
#define __WINCOMPAT_H__

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef uint32_t DWORD;
typedef int32_t LONG;
typedef uint8_t BYTE;
typedef int BOOL;
typedef char CHAR;
typedef char TCHAR;
typedef const char* LPCTSTR;

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

#define _T(x) x

struct RECT
{
    LONG left;
    LONG top;
    LONG right;
    LONG bottom;
};

#if !defined(_MSC_VER) && !defined(__forceinline)
#define __forceinline inline __attribute__((always_inline))
#endif

#ifndef _WIN32
#include <strings.h>

#define stricmp strcasecmp

inline char* itoa(int value, char* str, int radix)
{
    char digits[33];
    unsigned int v = value < 0 && radix == 10 ? 0u - (unsigned int)value : (unsigned int)value;
    int n = 0;
    char* p = str;

    do
    {
        digits[n++] = "0123456789abcdefghijklmnopqrstuvwxyz"[v % radix];
        v /= radix;
    }
    while (v);

    if (value < 0 && radix == 10)
    {
        *p++ = '-';
    }
    while (n)
    {
        *p++ = digits[--n];
    }
    *p = 0;

    return str;
}
#endif

#endif
