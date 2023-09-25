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

// [Cecil] This header defines platform-specific types
#ifndef SE_INCL_PLATFORMTYPES_H
#define SE_INCL_PLATFORMTYPES_H

#if SE1_WIN // Windows OS

typedef unsigned long int ULONG;
typedef   signed long int SLONG;

typedef long int RESULT; // For error codes
typedef long int INDEX;  // For indexed values and quantities

#elif SE1_UNIX // Unix OS

// 32-bit types
typedef unsigned int ULONG;
typedef   signed int SLONG;

typedef int RESULT; // For error codes
typedef int INDEX;  // For indexed values and quantities

// Windows types
typedef ULONG DWORD;
typedef SLONG LONG;
typedef UWORD WORD;
typedef ULONG WPARAM;
typedef SLONG LPARAM;
typedef DWORD COLORREF;
typedef void *LPVOID;

#if SE1_64BIT
  typedef UQUAD UINT_PTR;
  typedef SQUAD INT_PTR;
#else
  typedef ULONG UINT_PTR;
  typedef SLONG INT_PTR;
#endif

typedef UINT_PTR DWORD_PTR;

// [Cecil] TODO: Make sure these are properly used
typedef void *HWND;
typedef void *HINSTANCE;
typedef void *HGLRC;

typedef struct {
  LONG x;
  LONG y;
} POINT;

typedef struct {
  LONG left;
  LONG top;
  LONG right;
  LONG bottom;
} RECT;

// WAV format
#define WAVE_FORMAT_PCM 0x0001

#endif

#endif // include-once check
