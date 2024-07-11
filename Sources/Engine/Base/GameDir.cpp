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

#include <direct.h>

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
  GetModuleFileNameA(NULL, strPathBuffer, sizeof(strPathBuffer));

  // Cut off module filename to end up with the directory
  // E.g. "C:\\SeriousSam\\Bin\\x64"
  CTString strPath = strPathBuffer;
  strPath.Erase(strPath.RFind('\\'));

  // Find Bin folder with a platform name
  const char strBin86[] = "\\Bin\\x86";
  const char strBin64[] = "\\Bin\\x64";
  const size_t iDesiredPos = strPath.Length() - (sizeof(strBin86) - 1);

  size_t iPos = strPath.RFind(strBin86);

  // x86 isn't found at the end
  if (iPos != iDesiredPos) {
    iPos = strPath.RFind(strBin64);
  }

  // Cut off x86 or x64, if found at the end
  // E.g. "C:\\SeriousSam"
  if (iPos == iDesiredPos) {
    strPath.Erase(iPos);
  }

  // Get cut-off position before the Bin directory
  // E.g. between "C:\\SeriousSam\\" and "Bin\\x64\\SeriousSam.exe"
  const size_t iBinDir = strPath.Length() + 1;

  // Copy absolute path to the game directory and relative path to the executable
  CTString(strPathBuffer).Split(iBinDir, _fnmInternalAppPath, _fnmInternalAppExe);
};

// Create a series of directories within the game folder
void CreateAllDirectories(CTString strPath) {
  size_t iDir = 0;

  // Get next directory from the last position
  while ((iDir = strPath.Find('\\', iDir)) != CTString::npos) {
    // Include the slash
    iDir++;

    // Create current subdirectory
    int iDummy = _mkdir((_fnmApplicationPath + strPath.Substr(0, iDir)).ConstData());
    (void)iDummy;
  }
};
