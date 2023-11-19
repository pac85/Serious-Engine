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

// [Cecil] This header defines a cross-platform interface for interacting with operating systems
#ifndef SE_INCL_OS_H
#define SE_INCL_OS_H

// Compatibility with multiple platforms
#include <Engine/OS/PlatformSpecific.h>

// Non-Windows OS
#if !SE1_WIN

#define LOWORD(x) (x & 0xFFFF)
#define HIWORD(x) ((x >> 16) & 0xFFFF)

#endif // !SE1_WIN

class ENGINE_API OS {
  // Dynamic library methods
  public:

    // Load library
    static HMODULE LoadLib(const char *strLibrary);

    // Throw an error if unable to load a library
    static HMODULE LoadLibOrThrow_t(const char *strLibrary);

    // Free loaded library
    static BOOL FreeLib(HMODULE hLib);

    // Hook library symbol
    static void *GetLibSymbol(HMODULE hLib, const char *strSymbol);

  // Cross-platform reimplementations of methods from Windows API
  public:

    struct ENGINE_API Message {
      // PeekMessage()
      static BOOL Peek(MSG *lpMsg, void *hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax, UINT wRemoveMsg);

      // TranslateMessage()
      static void Translate(const MSG *lpMsg);

      // DispatchMessage()
      static void Dispatch(const MSG *lpMsg);
    };

    static BOOL IsIconic(void *hWnd);
    static UWORD GetKeyState(int vKey);
    static UWORD GetAsyncKeyState(int vKey);
    static BOOL GetCursorPos(LPPOINT lpPoint);
    static BOOL ScreenToClient(void *hWnd, LPPOINT lpPoint);
    static int ShowCursor(BOOL bShow);
};

#endif // include-once check
