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

#include "stdh.h"
#include <Engine/Base/Stream.h>
#include <Engine/Base/FileName.h>
#include <Engine/Base/Unzip.h>
#include <Engine/Templates/DynamicStackArray.cpp>

extern CDynamicStackArray<CTFileName> _afnmBaseBrowseInc;
extern CDynamicStackArray<CTFileName> _afnmBaseBrowseExc;

int qsort_CompareCTFileName(const void *elem1, const void *elem2 )
{
  const CTFileName &fnm1 = **(CTFileName **)elem1;
  const CTFileName &fnm2 = **(CTFileName **)elem2;
  return strcmp(fnm1.ConstData(), fnm2.ConstData());
}

void FillDirList_internal(CTFileName fnmBasePath,
  CDynamicStackArray<CTFileName> &afnm, const CTFileName &fnmDir, CTString strPattern, BOOL bRecursive,
  CDynamicStackArray<CTFileName> *pafnmInclude, CDynamicStackArray<CTFileName> *pafnmExclude)
{
#if !SE1_WIN
  // [Cecil] Fix path slashes
  fnmBasePath.ReplaceChar('\\', '/');
#endif

  // add the directory to list of directories to search
  CListHead lhDirs;
  new FileSystem::DirToRead(fnmDir, lhDirs);

  // while the list of directories is not empty
  while (!lhDirs.IsEmpty()) {
    // take the first one
    FileSystem::DirToRead *pdr = LIST_HEAD(lhDirs, FileSystem::DirToRead, lnInList);
    CTString fnmDir = pdr->strDir;
    delete pdr;

    // if the dir is not allowed
    if (pafnmInclude!=NULL &&
      (!FileMatchesList(*pafnmInclude, fnmDir) || FileMatchesList(*pafnmExclude, fnmDir)) ) {
      // skip it
      continue;
    }
    
    // start listing the directory
    FileSystem::Search search;
    BOOL bOK = search.FindFirst((fnmBasePath + fnmDir + "*").ConstData());

    // for each file in the directory
    while (bOK) {
      // Skip dummy directories (including parent) and files
      if (search.IsDummy()) {
        bOK = search.FindNext();
        continue;
      }

      // get the file's filepath
      CTString fnm = fnmDir + search.GetName();

      // if it is a directory
      if (FileSystem::IsDirectory((fnmBasePath + fnm).ConstData())) {
        // Search directories recursively
        if (bRecursive) {
          new FileSystem::DirToRead(fnm + "\\", lhDirs);
        }

      // Simply add the file if it matches the pattern
      } else {
        if (strPattern == "" || FileSystem::PathMatches(fnm, strPattern)) {
          afnm.Push() = fnm;
        }
      }

      bOK = search.FindNext();
    }
  }
}

// [Cecil] Moved code out of MakeDirList()
static void ListFromGRO(CDynamicStackArray<CTFileName> &afnmTemp, const CTFileName &fnmDir, const CTString &strPattern, ULONG ulFlags) {
  // [Cecil] Ignore packages
  if (ulFlags & DLI_IGNOREGRO) return;

  const BOOL bMod = (_fnmMod != "");
  const BOOL bRecursive = (ulFlags & DLI_RECURSIVE);
  const BOOL bLists = bMod && !(ulFlags & DLI_IGNORELISTS);

  const INDEX ctFilesInZips = UNZIPGetFileCount();

  for (INDEX iFileInZip = 0; iFileInZip < ctFilesInZips; iFileInZip++) {
    // Get ZIP entry
    const CZipEntry &ze = UNZIPGetEntry(iFileInZip);
    const CTFileName &fnm = ze.ze_fnm;

    // Skip if not under this directory
    if (bRecursive) {
      if (!fnm.HasPrefix(fnmDir)) continue;

    // Skip if not the same directory
    } else if (fnm.FileDir() != fnmDir) {
      continue;
    }

    // Doesn't match the pattern
    if (strPattern != "" && !fnm.Matches(strPattern)) continue;

    const BOOL bFileFromMod = UNZIPIsFileAtIndexMod(iFileInZip);

    // List files exclusively from the mod
    if (ulFlags & DLI_ONLYMOD) {
      if (bMod && bFileFromMod) {
        afnmTemp.Push() = fnm;
      }

    // List files from the game
    } else if (!bFileFromMod) {
      // Not a mod file or shouldn't match mod's browse paths
      if (!bLists) {
        afnmTemp.Push() = fnm;

      // Matches mod's browse paths
      } else if (FileMatchesList(_afnmBaseBrowseInc, fnm) && FileMatchesList(_afnmBaseBrowseExc, fnm)) {
        afnmTemp.Push() = fnm;
      }

    // List extras from the mod
    } else if (!(ulFlags & DLI_IGNOREMOD) && bMod) {
      afnmTemp.Push() = fnm;
    }
  }
};

// make a list of all files in a directory
ENGINE_API void MakeDirList(
  CDynamicStackArray<CTFileName> &afnmDir, const CTFileName &fnmDir, const CTString &strPattern, ULONG ulFlags)
{
  // Make a temporary list
  CDynamicStackArray<CTFileName> afnmTemp;

  // [Cecil] Reuse the file list
  if (ulFlags & DLI_REUSELIST) {
    afnmTemp = afnmDir;
  }

  // Clear the final list
  afnmDir.PopAll();

  const BOOL bMod = (_fnmMod != "");
  const BOOL bRecursive = (ulFlags & DLI_RECURSIVE);

  // [Cecil] Determine whether browse lists from a mod should be used
  const BOOL bLists = bMod && !(ulFlags & DLI_IGNORELISTS);
  CDynamicStackArray<CTString> *paBaseBrowseInc = (bLists ? &_afnmBaseBrowseInc : NULL);
  CDynamicStackArray<CTString> *paBaseBrowseExc = (bLists ? &_afnmBaseBrowseExc : NULL);

  // [Cecil] List files exclusively from GRO packages (done afterwards)
  if (ulFlags & DLI_ONLYGRO) {
    NOTHING;

  // [Cecil] List files exclusively from the mod
  } else if (ulFlags & DLI_ONLYMOD) {
    if (bMod) {
      FillDirList_internal(_fnmApplicationPath + _fnmMod, afnmTemp, fnmDir, strPattern, bRecursive, NULL, NULL);
    }

  } else {
    // List files from the game directory
    FillDirList_internal(_fnmApplicationPath, afnmTemp, fnmDir, strPattern, bRecursive, paBaseBrowseInc, paBaseBrowseExc);

    // List extra files from the CD
    if (ulFlags & DLI_SEARCHCD && _fnmCDPath != "") {
      FillDirList_internal(_fnmCDPath, afnmTemp, fnmDir, strPattern, bRecursive, paBaseBrowseInc, paBaseBrowseExc);
    }

    // List extra files from the mod directory
    if (!(ulFlags & DLI_IGNOREMOD) && bMod) {
      FillDirList_internal(_fnmApplicationPath + _fnmMod, afnmTemp, fnmDir, strPattern, bRecursive, NULL, NULL);
    }
  }

  // [Cecil] Search for files in the packages
  ListFromGRO(afnmTemp, fnmDir, strPattern, ulFlags);

  const INDEX ctFiles = afnmTemp.Count();

  // Don't check for duplicates if no files
  if (ctFiles == 0) return;

  // Sort the file list
  qsort(afnmTemp.da_Pointers, afnmTemp.Count(), sizeof(CTFileName *), qsort_CompareCTFileName);

  // Copy the first file into the final list
  afnmDir.Push() = afnmTemp[0];

  // Copy the rest of the files if they aren't matching previous files
  for (INDEX iFile = 1; iFile < ctFiles; iFile++)
  {
    if (afnmTemp[iFile] != afnmTemp[iFile - 1]) {
      afnmDir.Push() = afnmTemp[iFile];
    }
  }
};
