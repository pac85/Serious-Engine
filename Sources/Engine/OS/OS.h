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

// Unix compatibility
#if SE1_UNIX
  typedef void *HINSTANCE;
  typedef void *HMODULE;
#endif

class OS {
  public:
    // Load library
    static HMODULE LoadLib(const char *strLibrary);

    // Free loaded library
    static BOOL FreeLib(HMODULE hLib);

    // Hook library symbol
    static void *GetLibSymbol(HMODULE hLib, const char *strSymbol);
};

#endif // include-once check
