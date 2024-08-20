/* Copyright (c) 2024 Dreamy Cecil
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

#if SE1_WIN

#include <direct.h>

#else

#include <sys/stat.h>

// [Cecil] Stolen from https://github.com/icculus/Serious-Engine
// Stolen from SDL2/src/filesystem/unix/SDL_sysfilesystem.c
static char *readSymLink(const char *path) {
  char *retval = NULL;
  ssize_t len = 64;
  ssize_t rc = -1;

  while (1) {
    char *ptr = (char *)SDL_realloc(retval, (size_t)len);

    if (ptr == NULL) {
      SDL_OutOfMemory();
      break;
    }

    retval = ptr;
    rc = readlink(path, retval, len);

    if (rc == -1) {
      break; // not a symlink, i/o error, etc.

    } else if (rc < len) {
      retval[rc] = '\0'; // readlink doesn't null-terminate.

      // try to shrink buffer...
      ptr = (char *)SDL_realloc(retval, strlen(retval) + 1);

      if (ptr != NULL) {
        retval = ptr; // oh well if it failed.
      }

      return retval; // we're good to go.
    }

    len *= 2; // grow buffer, try again.
  }

  SDL_free(retval);
  return NULL;
};

#endif // SE1_WIN

// Global string with an absolute path to the main game directory
static CTFileName _fnmInternalAppPath;

// Global string with a relative path to the filename of the started application
static CTFileName _fnmInternalAppExe;

// References to internal variables
const CTFileName &_fnmApplicationPath = _fnmInternalAppPath;
const CTFileName &_fnmApplicationExe = _fnmInternalAppExe;

// Determine application paths for the first time
void DetermineAppPaths(void) {
  // Get full path to the executable module
  // E.g. "C:\\SeriousSam\\Bin\\x64\\SeriousSam.exe"
  char strPathBuffer[1024];

#if SE1_WIN
  GetModuleFileNameA(NULL, strPathBuffer, sizeof(strPathBuffer));
#else
  char *strExePath = readSymLink("/proc/self/exe");
  strcpy(strPathBuffer, strExePath);
  SDL_free(strExePath);
#endif

  CTString strPath(strPathBuffer);
  strPath.ReplaceChar('/', '\\');

  // Cut off module filename to end up with the directory
  // E.g. "C:\\SeriousSam\\Bin\\x64"
  strPath.Erase(strPath.RFind('\\'));

  // Check if there's a Bin folder in the middle
  size_t iPos = strPath.RFind("\\Bin\\");

  if (iPos != CTString::npos) {
    strPath.Erase(iPos);

  } else {
    // Check if there's a Bin folder at the end
    iPos = strPath.RFind("\\Bin");

    if (iPos == strPath.Length() - 4) {
      strPath.Erase(iPos);
    }
  }

  // Get cut-off position before the Bin directory
  // E.g. between "C:\\SeriousSam\\" and "Bin\\x64\\SeriousSam.exe"
  const size_t iBinDir = strPath.Length() + 1;

  // Copy absolute path to the game directory and relative path to the executable
  CTString(strPathBuffer).Split((INDEX)iBinDir, _fnmInternalAppPath, _fnmInternalAppExe);
};

// Create a series of directories within the game folder
void CreateAllDirectories(CTString strPath) {
  size_t iDir = 0;

  // Get next directory from the last position
  while ((iDir = strPath.Find('\\', iDir)) != CTString::npos) {
    // Include the slash
    iDir++;

    // Create current subdirectory
    CTString strDir = _fnmApplicationPath + strPath.Substr(0, iDir);
    strDir.ReplaceChar('\\', '/'); // [Cecil] NOTE: For _mkdir()

    int iDummy = _mkdir(strDir.ConstData());
    (void)iDummy;
  }
};
