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

#include "StdH.h"

#include "OS.h"

#if SE1_WIN

// Load library
HMODULE OS::LoadLib(const char *strLibrary) {
  return ::LoadLibraryA(strLibrary);
};

// Free loaded library
BOOL OS::FreeLib(HMODULE hLib) {
  return ::FreeLibrary(hLib);
};

// Hook library symbol
void *OS::GetLibSymbol(HMODULE hLib, const char *strSymbol) {
  return ::GetProcAddress(hLib, strSymbol);
};

#else

#include <dlfcn.h>

// Load library
HMODULE OS::LoadLib(const char *strLibrary) {
  CTFileName fnmPath = CTString(strLibrary);

  // Prepend "lib"
  const CTString strFileName = fnmPath.FileName();

  if (!strFileName.HasPrefix("lib")) {
    fnmPath = fnmPath.FileDir() + "lib" + strFileName + ".so";

  // Replace extension
  } else if (fnmPath.FileExt() == ".dll") {
    fnmPath = fnmPath.NoExt() + ".so";
  }

  // Fix path slashes
  fnmPath.ReplaceChar('\\', '/');

  return dlopen(fnmPath.ConstData(), RTLD_LAZY | RTLD_GLOBAL);
};

// Free loaded library
BOOL OS::FreeLib(HMODULE hLib) {
  return dlclose(hLib);
};

// Hook library symbol
void *OS::GetLibSymbol(HMODULE hLib, const char *strSymbol) {
  return dlsym(hLib, strSymbol);
};

#endif

// Throw an error if unable to load a library
HMODULE OS::LoadLibOrThrow_t(const char *strLibrary) {
  HMODULE hLib = OS::LoadLib(strLibrary);

  if (hLib == NULL) {
    ThrowF_t(TRANS("Cannot load DLL file '%s':\n%s"), strLibrary, GetWindowsError(GetLastError()));
  }

  return hLib;
};
