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

// [Cecil] This header defines a cross-platform interface for working with files and directories on a hard drive
#ifndef SE_INCL_FILESYSTEM_H
#define SE_INCL_FILESYSTEM_H

#if SE1_WIN
  #include <io.h>
#else
  #include <dirent.h>
#endif

class ENGINE_API FileSystem {
  public:

    // Temporary directory for listing files
    struct ENGINE_API DirToRead
    {
      CListNode lnInList;
      CTString strDir;

      DirToRead(const CTString &strSetDir, CListHead &lhAddToList) {
        strDir = strSetDir;

      #if !SE1_WIN
        // Fix path slashes
        strDir.ReplaceChar('\\', '/');
      #endif

        lhAddToList.AddTail(lnInList);
      };
    };

    // Temporary structure for iterating through files
    struct ENGINE_API Search
    {
      #if SE1_WIN
        _finddata_t fdSearch; // Search data
        INT_PTR hFile; // Current file handle
      #else
        DIR *dDir; // Directory to search through
        dirent *dent; // Current directory entry
      #endif

      Search();
      ~Search();

      // Find first file in a directory
      BOOL FindFirst(const char *strDir);

      // Find next file
      BOOL FindNext(void);

      // Check if it's a dummy/parent directory
      BOOL IsDummy(void) const;

      // Get name of the current file/directory
      const char *GetName(void) const;
    };

  public:

    // Check if specified filename exists on disk
    static BOOL Exists(const char *strFilename);

    // Check if specified filename is a directory
    static BOOL IsDirectory(const char *strFilename);

    // Check if some path matches a wildcard (should be prioritized over simple CTString::Matches() method)
    static BOOL PathMatches(CTString strPath, CTString strMatch);
};

#endif // include-once check
