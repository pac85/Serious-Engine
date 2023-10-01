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


// make a list of all files in a directory
ENGINE_API void MakeDirList(
  CDynamicStackArray<CTFileName> &afnmDir, const CTFileName &fnmDir, const CTString &strPattern, ULONG ulFlags)
{
  afnmDir.PopAll();
  BOOL bRecursive = ulFlags&DLI_RECURSIVE;
  BOOL bSearchCD  = ulFlags&DLI_SEARCHCD;

  // make one temporary array
  CDynamicStackArray<CTFileName> afnm;

  if (_fnmMod!="") {
    FillDirList_internal(_fnmApplicationPath, afnm, fnmDir, strPattern, bRecursive,
      &_afnmBaseBrowseInc, &_afnmBaseBrowseExc);
    if (bSearchCD) {
      FillDirList_internal(_fnmCDPath, afnm, fnmDir, strPattern, bRecursive,
      &_afnmBaseBrowseInc, &_afnmBaseBrowseExc);
    }
    FillDirList_internal(_fnmApplicationPath+_fnmMod, afnm, fnmDir, strPattern, bRecursive, NULL, NULL);
  } else {
    FillDirList_internal(_fnmApplicationPath, afnm, fnmDir, strPattern, bRecursive, NULL, NULL);
    if (bSearchCD) {
      FillDirList_internal(_fnmCDPath, afnm, fnmDir, strPattern, bRecursive, NULL, NULL);
    }
  }

  // for each file in zip archives
  CTString strDirPattern = fnmDir;
  INDEX ctFilesInZips = UNZIPGetFileCount();
  for(INDEX iFileInZip=0; iFileInZip<ctFilesInZips; iFileInZip++) {
    const CTFileName &fnm = UNZIPGetFileAtIndex(iFileInZip);

    // if not in this dir, skip it
    if (bRecursive) {
      if (!fnm.HasPrefix(strDirPattern)) {
        continue;
      }
    } else {
      if (fnm.FileDir()!=fnmDir) {
        continue;
      }
    }

    // Skip if doesn't match the pattern
    if (strPattern != "" && !FileSystem::PathMatches(fnm, strPattern)) {
      continue;
    }

    // if mod is active, and the file is not in mod
    if (_fnmMod!="" && !UNZIPIsFileAtIndexMod(iFileInZip)) {
      // if it doesn't match base browse path
      if ( !FileMatchesList(_afnmBaseBrowseInc, fnm) || FileMatchesList(_afnmBaseBrowseExc, fnm) ) {
        // skip it
        continue;
      }
    }

    // add that file
    afnm.Push() = fnm;
  }

  // if no files
  if (afnm.Count()==0) {
    // don't check for duplicates
    return;
  }

  // resort the array
  qsort(afnm.da_Pointers, afnm.Count(), sizeof(void*), qsort_CompareCTFileName);

  // for each file
  INDEX ctFiles = afnm.Count();
  for (INDEX iFile=0; iFile<ctFiles; iFile++) {
    // if not same as last one
    if (iFile==0 || afnm[iFile]!=afnm[iFile-1]) {
      // copy over to final array
      afnmDir.Push() = afnm[iFile];
    }
  }
}
