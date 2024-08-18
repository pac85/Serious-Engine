/* Copyright (c) 2023 Dreamy Cecil
This program is free software; you can redistribute it and/or modify
it under the terms of version 2 of the GNU General Public License as published by
the Free Software Foundation


This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA. */

// [Cecil] This header defines specific features that may be missing from certain platforms
#ifndef SE_INCL_PLATFORMSPECIFIC_H
#define SE_INCL_PLATFORMSPECIFIC_H

// Thread-local variables specifier
#ifndef SE1_THREADLOCAL
  #if SE1_SINGLE_THREAD
    #define SE1_THREADLOCAL
  #elif SE1_WIN
    #define SE1_THREADLOCAL __declspec(thread)
  #elif __STDC_VERSION__ >= 201112L
    #define SE1_THREADLOCAL _Thread_local
  #else
    #define SE1_THREADLOCAL __thread
  #endif
#endif // SE1_THREADLOCAL

// 'noexcept' doesn't work in MSVC 12.0 and prior
#if defined(_MSC_VER) && _MSC_VER <= 1800
  #define SE1_NOEXCEPT
#else
  #define SE1_NOEXCEPT noexcept
#endif

// Formatting function attribute (NOTE: add 1 to argument numbers if using this on class methods to skip 'this' argument)
#define SE1_FORMAT_FUNC(_FormatArg, _VariadicArgs) __attribute__((format(printf, _FormatArg, _VariadicArgs)))

// Windows-specific
#if SE1_WIN

// Ignore GCC attributes
#define __attribute__(x)

#endif // SE1_WIN

// Unix-specific
#if SE1_UNIX

// Internal
#define __forceinline __attribute__((always_inline)) inline
#define __stdcall
#define __cdecl
#define WINAPI

#ifndef MAX_PATH
  #ifdef MAXPATHLEN
    #define MAX_PATH MAXPATHLEN
  #else
    #define MAX_PATH 256
  #endif
#endif

#define _O_BINARY 0
#define _O_RDONLY O_RDONLY
#define _S_IWRITE S_IWRITE
#define _S_IREAD  S_IREAD

// Macros for Windows methods
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdarg.h>
#include <ctype.h>
#include <math.h>

#define _snprintf snprintf
#define _vsnprintf vsnprintf
#define stricmp strcasecmp
#define strnicmp strncasecmp

#define _CrtCheckMemory() 1
#define _mkdir(x) mkdir(x, S_IRWXU)
#define _open open
#define _close close
#define _set_new_handler std::set_new_handler
#define _finite isfinite

inline void _strupr(char *str) {
  if (str == NULL) return;

  while (*str != '\0') {
    *str = static_cast<char>(toupper(static_cast<UBYTE>(*str)));
    ++str;
  }
};

// Windows reports
inline void _RPT_do(const char *type, const char *fmt, ...)
{
#ifndef NDEBUG
  va_list ap;
  fprintf(stderr, "_RPT (%s): ", type);
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  va_end(ap);
  fflush(stderr);
#endif
};

#define _CRT_WARN "_CRT_WARN"
#define _CRT_ERROR "_CRT_ERROR"
#define _CRT_ASSER "_CRT_ASSERT"

#define _RPT0(type, fmt)                 _RPT_do(type, fmt)
#define _RPT1(type, fmt, a1)             _RPT_do(type, fmt, a1)
#define _RPT2(type, fmt, a1, a2)         _RPT_do(type, fmt, a1, a2)
#define _RPT3(type, fmt, a1, a2, a3)     _RPT_do(type, fmt, a1, a2, a3)
#define _RPT4(type, fmt, a1, a2, a3, a4) _RPT_do(type, fmt, a1, a2, a3, a4)

// Networking
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define closesocket close
#define WSAGetLastError() (INDEX)errno

typedef int SOCKET;
typedef hostent HOSTENT;
typedef sockaddr_in SOCKADDR_IN;
typedef sockaddr    SOCKADDR;

#endif // SE1_UNIX

#endif // include-once check
