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

// [Cecil] This header determines used platform and operating system and defines useful macro switches
#ifndef SE_INCL_CONFIG_H
#define SE_INCL_CONFIG_H

// Processor architectures (SE1_PLATFORM)
#define PLATFORM_X86 0
#define PLATFORM_X64 1

// Operating systems (SE1_SYSTEM)
#define OS_WINDOWS 0
#define OS_UNIX    1

// Determine architecture
#if defined(__x86_64__) || defined(_M_X64)
  #define SE1_PLATFORM PLATFORM_X64

#elif defined(i386) || defined(__i386__) || defined(__i386) || defined(_M_IX86)
  #define SE1_PLATFORM PLATFORM_X86

#else
  #error Architecture unsupported!
  #define SE1_PLATFORM -1
#endif

// Determine system
#if defined(_MSC_VER) || defined(_WIN32) || defined(_WIN64)
  #define SE1_SYSTEM OS_WINDOWS

#elif defined(__GNUC__) || defined(__linux__)
  #define SE1_SYSTEM OS_UNIX

#else
  #error Operating system unsupported!
  #define SE1_SYSTEM -1
#endif

// Simplistic platform switches
#define SE1_32BIT (SE1_PLATFORM == PLATFORM_X86) // Is building under 32-bit platform?
#define SE1_64BIT (SE1_PLATFORM == PLATFORM_X64) // Is building under 64-bit platform?

#define SE1_WIN  (SE1_SYSTEM == OS_WINDOWS) // Is building under Windows OS?
#define SE1_UNIX (SE1_SYSTEM == OS_UNIX)    // Is building under Unix OS?

// Check if inline assembly is prefered
#ifndef SE1_ASMOPT
  #define SE1_ASMOPT 1
#endif

// Check if texture dithering should be enabled
#ifndef SE1_DITHERBITMAP
  #define SE1_DITHERBITMAP 1
#endif

// Don't prioritize SDL functionality over Windows API by default
#if SE1_WIN
  // Needs SE1_SDL to be defined as well to work
  #ifndef SE1_USE_SDL
    #define SE1_USE_SDL 0
  #endif

#else
  #define SE1_SDL // Should always be enabled
  #define SE1_USE_SDL 1
#endif

// Building under a traditional platform (Windows x86)
#if SE1_WIN && SE1_32BIT
  #define SE1_OLD_COMPILER (_MSC_VER < 1600) // Check for the old compiler
  #define SE1_USE_ASM (SE1_ASMOPT) // Check if able to prioritize inline assembly

#else
  #define SE1_OLD_COMPILER 0 // Not an old compiler
  #define SE1_USE_ASM 0 // Cannot use inline assembly
#endif

// [Cecil] NOTE: Experimental
#if !defined(SE1_MMXINTOPT) && SE1_32BIT
  #define SE1_MMXINTOPT 1
#endif

// Determine if using compilers with incomplete support of C++11
#if defined(_MSC_VER) && _MSC_VER < 1800
  #define SE1_INCOMPLETE_CPP11 1
#else
  #define SE1_INCOMPLETE_CPP11 0
#endif

#endif // include-once check
