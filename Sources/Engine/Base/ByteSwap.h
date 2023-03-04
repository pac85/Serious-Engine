/* Copyright (c) 2021-2022 by ZCaliptium.
   Copyright (c) 2023 Dreamy Cecil

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

#ifndef SE_INCL_BYTESWAP_H
#define SE_INCL_BYTESWAP_H

#ifdef PRAGMA_ONCE
  #pragma once
#endif

#include <stdlib.h>

template<typename ValType>
inline ValType ByteSwapT(ValType s)
{
  typedef struct 
  {
    union {
      ValType val;
      char b[sizeof(ValType)];
    };
  } SwapData;

  SwapData src;
  SwapData dst;
  src.val = s;

  for (int i = 0; i < sizeof(ValType); i++)
  {
    dst.b[i] = src.b[sizeof(ValType) - i - 1];
  }

  return dst.val;
};

#if SE1_WIN
  // [Cecil] Define byte-swapping methods for older compilers
  #if SE1_OLD_COMPILER
    __forceinline UWORD _byteswap_ushort(UWORD x) {
      return ((x >> 8) & 0xFFu) | ((x & 0xFFu) << 8);
    };

    __forceinline ULONG _byteswap_ulong(ULONG x) {
      ULONG ulRet;

      __asm {
        mov   eax, dword ptr [x]
        bswap eax
        mov   dword ptr [ulRet], eax
      }

      return ulRet;
    };

    __forceinline UQUAD _byteswap_uint64(UQUAD x) {
      return ByteSwapT<UQUAD>(x);
    };
  #endif // SE1_OLD_COMPILER

  #define ByteSwap16(x) _byteswap_ushort((x))
  #define ByteSwap32(x) _byteswap_ulong((x))
  #define ByteSwap64(x) _byteswap_uint64((x))

#elif (__GNUC__ > 4) || (__GNUC__ == 4 && __GNUC_MINOR__ >= 8)
  #define ByteSwap16(x) __builtin_bswap16((x))
  #define ByteSwap32(x) __builtin_bswap32((x))
  #define ByteSwap64(x) __builtin_bswap64((x))
#endif

#endif