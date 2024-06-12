/* Copyright (c) 2002-2012 Croteam Ltd. 
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

// [Cecil] Declaration specifiers for exporting and importing symbols from modules
#if SE1_WIN
  #define SE1_API_EXPORT __declspec(dllexport)
  #define SE1_API_IMPORT __declspec(dllimport)
#else
  #define SE1_API_EXPORT __attribute__((visibility("default")))
  #define SE1_API_IMPORT
#endif

#if SE1_WIN
  #ifndef PRAGMA_ONCE
    #define PRAGMA_ONCE
  #endif

  // disable problematic warnings
  #pragma warning(disable: 4251)  // dll interfacing problems
  #pragma warning(disable: 4275)  // dll interfacing problems
  #pragma warning(disable: 4018)  // signed/unsigned mismatch
  #pragma warning(disable: 4244)  // type conversion warnings
  #pragma warning(disable: 4284)  // using -> for UDT
  #pragma warning(disable: 4355)  // 'this' : used in base member initializer list
  #pragma warning(disable: 4660)  // template-class specialization is already instantiated
  #pragma warning(disable: 4723)  // potential divide by 0
#endif

// Define engine API
#ifdef ENGINE_EXPORTS
  #define ENGINE_API SE1_API_EXPORT
#else
  #define ENGINE_API SE1_API_IMPORT

  #ifdef NDEBUG
    #pragma comment(lib, "Engine.lib")
  #else
    #pragma comment(lib, "EngineD.lib")
  #endif
#endif
